#include "session.h"

#include <cassert>

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/Dynamic/Parser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "clang/Frontend/ASTUnit.h"

using namespace clang;
using namespace tooling;
using namespace ast_matchers;

Session::Session(const std::string& databasePath) {
	std::string error;
	compilationDatabase = std::unique_ptr<JSONCompilationDatabase>(
		JSONCompilationDatabase::loadFromFile(databasePath, error));

	assert(error.empty() && "TODO: handle this error");

	files = compilationDatabase->getAllFiles();
	tool = std::auto_ptr<clang::tooling::ClangTool>(new ClangTool(*compilationDatabase, files));

	int ret = tool->buildASTs(ASTlist);
	assert(!ret && "TODO: handle this error");
}

Session::~Session() {}

namespace {

struct CollectBoundNodes : MatchFinder::MatchCallback {
	std::vector<BoundNodes> &bindings;

	CollectBoundNodes(std::vector<BoundNodes> &bindings) : bindings(bindings) {}

	void run(const MatchFinder::MatchResult &result) {
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
	std::string entry;

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

				llvm::StringRef fileName = srcMgr.getFilename(start);

				if (fileName.empty()) continue;

				entry = fileName.str();
				entry += " (" + idToNode.first + "): ";
				entry += std::to_string(srcMgr.getSpellingLineNumber(start)) + ",";
				entry += std::to_string(srcMgr.getSpellingColumnNumber(start));

				foundMatches.insert(entry);
			}
		}
	}
}

std::set<std::string> Session::getMatches() {
	return foundMatches;
}