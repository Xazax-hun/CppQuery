#include "session.h"

#include <fstream>
#include <tuple>

#include <cassert>

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/Dynamic/Parser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Lex/Lexer.h"

using namespace clang;
using namespace tooling;
using namespace ast_matchers;

using namespace CppQuery;

namespace {

class ResourceDirSetter : public ArgumentsAdjuster {
public:
  ResourceDirSetter(const std::string& s) : resourceDir(s) {}

  CommandLineArguments Adjust(const CommandLineArguments &args) {
    CommandLineArguments adjustedArgs(args);
    adjustedArgs.push_back("-resource-dir="+resourceDir);
    return adjustedArgs;
  }
private:
  std::string resourceDir;
};

class ASTBuilderAction : public ToolAction {
public:
  ASTBuilderAction(std::vector<ASTUnit *> &ASTlist,
                   const std::function<bool(const std::string &)> &onTUend)
      : ASTlist(ASTlist), onTUend(onTUend) {
    errorNum = 0;
  }

  bool runInvocation(CompilerInvocation *invocation, FileManager *files,
                     DiagnosticConsumer *diagConsumer) {
    TextDiagnosticBuffer diag;
    ASTUnit *AST = ASTUnit::LoadFromCompilerInvocation(
        invocation, CompilerInstance::createDiagnostics(
                        &invocation->getDiagnosticOpts(), &diag, false));

    if (!AST)
      return false;

    if (onTUend(AST->getMainFileName().str())) {
      ASTlist.push_back(AST);
      SourceManager &srcMgr = AST->getSourceManager();
      for (TextDiagnosticBuffer::DiagList::const_iterator it = diag.err_begin();
           it != diag.err_end(); ++it) {
        errors.push_back(
            srcMgr.getFilename(it->first).str() + " line: " +
            std::to_string(srcMgr.getSpellingLineNumber(it->first)) + ": " +
            it->second + "\n");
      }
    }

    return true;
  }

  std::vector<std::string>::const_iterator errBegin() { return errors.begin(); }
  std::vector<std::string>::const_iterator errEnd() { return errors.end(); }

private:
  unsigned errorNum;
  std::vector<std::string> errors;
  std::vector<ASTUnit *> &ASTlist;
  const std::function<bool(const std::string &)> &onTUend;
};
}

bool CppQuery::operator<(const Match &lhs, const Match &rhs) {
  return std::tie(lhs.fileName, lhs.id, lhs.startLine, lhs.startCol,
                  lhs.endLine,
                  lhs.endCol) < std::tie(rhs.fileName, rhs.id, rhs.startLine,
                                         rhs.startCol, rhs.endLine, rhs.endCol);
}

std::ostream &CppQuery::operator<<(std::ostream &s, const Match &m) {
  s << m.fileName << "(" << m.id << "):" << m.startLine << "," << m.startCol
    << ":" << m.endLine << "," << m.endCol << std::endl;
  return s;
}

Session::Session(const std::string &databasePath,
                 const std::string &resourceDir) {
  std::string error;
  compilationDatabase = std::unique_ptr<JSONCompilationDatabase>(
      JSONCompilationDatabase::loadFromFile(databasePath, error));

  if (!error.empty())
    throw DatabaseError(error);

  files = compilationDatabase->getAllFiles();
  tool = std::unique_ptr<clang::tooling::ClangTool>(
      new ClangTool(*compilationDatabase, files));

  tool->appendArgumentsAdjuster(new ResourceDirSetter(resourceDir));
}

Session::~Session() {
  for (auto AST : ASTlist) {
    AST->setUnsafeToFree(false);
    delete AST;
  }
  tool->clearArgumentsAdjusters();
}

void
Session::parseFiles(const std::function<bool(const std::string &)> &onTUend) {
  ASTBuilderAction action(ASTlist, onTUend);

  if (tool->run(&action) || action.errBegin() != action.errEnd()) {
    std::string errors;
    for (auto it = action.errBegin(); it != action.errEnd(); ++it) {
      errors += *it;
    }
    throw ParseError(errors);
  }
}

namespace {

struct CollectBoundNodes : MatchFinder::MatchCallback {
  std::vector<BoundNodes> &bindings;

  CollectBoundNodes(std::vector<BoundNodes> &bindings) : bindings(bindings) {}

  void run(const MatchFinder::MatchResult &result) {
    bindings.push_back(result.Nodes);
  }
};
}

void Session::runQuery(const std::string &query) {
  if (query.find(".bind") == std::string::npos)
    throw QueryError("There is no such node that was bound to an ID.");

  if (ASTlist.empty())
    throw QueryError("There is no file that was succesfully parsed.");

  dynamic::Diagnostics diag;
  llvm::Optional<dynamic::DynTypedMatcher> matcher =
      dynamic::Parser::parseMatcherExpression(query, &diag);

  if (!matcher)
    throw QueryError(diag.toString());

  foundMatches.clear();
  Match m;

  for (auto ast : ASTlist) {
    SourceManager &srcMgr = ast->getSourceManager();

    MatchFinder finder;
    std::vector<BoundNodes> boundNodes;
    CollectBoundNodes collector(boundNodes);

    if (!finder.addDynamicMatcher(*matcher, &collector))
      throw QueryError("Invalid top level matcher.");

    finder.matchAST(ast->getASTContext());

    for (auto nodes : boundNodes) {
      for (auto idToNode : nodes.getMap()) {
        SourceRange range = idToNode.second.getSourceRange();

        if (!range.isValid())
          continue;

        SourceLocation start = range.getBegin();

        if (srcMgr.isInSystemHeader(start) ||
            srcMgr.isInExternCSystemHeader(start))
          continue;

        // Get the last character of the last token
        SourceLocation end = Lexer::getLocForEndOfToken(range.getEnd(), 1,
                                                        srcMgr, LangOptions());

        llvm::StringRef fileName = srcMgr.getFilename(start);

        if (fileName.empty())
          continue;

        m.fileName = fileName.str();
        m.id = idToNode.first;
        m.startCol = srcMgr.getSpellingColumnNumber(start);
        m.startLine = srcMgr.getSpellingLineNumber(start);
        m.endCol = srcMgr.getSpellingColumnNumber(end);
        m.endLine = srcMgr.getSpellingLineNumber(end);

        foundMatches.insert(std::move(m));
      }
    }
  }
}

const std::set<Match> &Session::getMatches() const { return foundMatches; }

bool Session::exportMatches(const std::string &fileName) {
  std::ofstream output(fileName);

  if (!output)
    return false;

  for (const Match &m : foundMatches) {
    output << m;
  }

  return true;
}
