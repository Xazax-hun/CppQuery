#include "session.h"

#include <tuple>
#include <cassert>

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/Dynamic/Parser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Lexer.h"

using namespace clang;
using namespace tooling;
using namespace ast_matchers;

bool operator<(const Match& lhs, const Match& rhs) {
	return std::tie(lhs.fileName, lhs.id, lhs.startLine, lhs.startCol, lhs.endLine, lhs.endCol) <
		std::tie(rhs.fileName, rhs.id, rhs.startLine, rhs.startCol, rhs.endLine, rhs.endCol);
}

Session::Session(const std::string& databasePath) {
	std::string error;
	compilationDatabase = std::unique_ptr<JSONCompilationDatabase>(
		JSONCompilationDatabase::loadFromFile(databasePath, error));

	if (!error.empty())
		throw DatabaseError(error);

	files = compilationDatabase->getAllFiles();
	tool = std::unique_ptr<clang::tooling::ClangTool>(new ClangTool(*compilationDatabase, files));
}

Session::~Session() {
	for(auto AST : ASTlist) {
		AST->setUnsafeToFree(false);
		delete AST;
	}
}

namespace {
class ASTBuilderAction : public ToolAction {
public:
	ASTBuilderAction(std::vector<ASTUnit*>& ASTlist, const std::function<bool(const std::string&)>& onTUend)
		: ASTlist(ASTlist), onTUend(onTUend) {}

	bool runInvocation(CompilerInvocation* invocation, FileManager* files, DiagnosticConsumer* diagConsumer) {
		ASTUnit *AST = ASTUnit::LoadFromCompilerInvocation(invocation,
			CompilerInstance::createDiagnostics(&invocation->getDiagnosticOpts(), diagConsumer, false));

		if (!AST) return false;

		if(onTUend(AST->getMainFileName().str()))
			ASTlist.push_back(AST);

		return true;
	}

private:
	std::vector<ASTUnit*>& ASTlist;
	const std::function<bool(const std::string&)>& onTUend;
};

}

void Session::parseFiles(const std::function<bool(const std::string&)>& onTUend) {
	ASTBuilderAction action(ASTlist, onTUend);
	int ret = tool->run(&action);
	assert(!ret && "TODO: handle this error");
}

namespace {

struct CollectBoundNodes : MatchFinder::MatchCallback {
	std::vector<BoundNodes>& bindings;

	CollectBoundNodes(std::vector<BoundNodes>& bindings) : bindings(bindings) {}

	void run(const MatchFinder::MatchResult& result) {
		bindings.push_back(result.Nodes);
	}
};

}

void Session::runQuery(const std::string& query) {
	dynamic::Diagnostics diag;
	llvm::Optional<dynamic::DynTypedMatcher> matcher = dynamic::Parser::parseMatcherExpression(query, &diag);

	if (!matcher) {
		assert(false && "Unimplemented");
		return;
	}

	foundMatches.clear();
	Match m;

	for(auto ast : ASTlist) {
		SourceManager& srcMgr = ast->getSourceManager();

		MatchFinder finder;
		std::vector<BoundNodes> boundNodes;
		CollectBoundNodes collector(boundNodes);

		if(!finder.addDynamicMatcher(*matcher, &collector))
			assert(false && "Unimplemented");

		finder.matchAST(ast->getASTContext());

		for(auto nodes : boundNodes) {
			for(auto idToNode : nodes.getMap()) {
				SourceRange range = idToNode.second.getSourceRange();

				if (!range.isValid()) continue;

				SourceLocation start = range.getBegin();

				if (srcMgr.isInSystemHeader(start) ||
					srcMgr.isInExternCSystemHeader(start))
					continue;

				// Get the last character of the last token
				SourceLocation end = Lexer::getLocForEndOfToken(range.getEnd(), 1, srcMgr, LangOptions());

				llvm::StringRef fileName = srcMgr.getFilename(start);

				if (fileName.empty()) continue;

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

const std::set<Match>& Session::getMatches() const {
	return foundMatches;
}
