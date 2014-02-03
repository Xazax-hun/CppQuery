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
using namespace dynamic;

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
	Diagnostics diag;
	llvm::Optional<DynTypedMatcher> matcher = Parser::parseMatcherExpression(query, &diag);

	if (!matcher) {
		assert(false && "Unimplemented");
		return;
	}

	for(auto ast : ASTlist) {
		MatchFinder finder;
		std::vector<BoundNodes> matches;
		CollectBoundNodes collector(matches);

		if(!finder.addDynamicMatcher(*matcher, &collector))
			assert(false && "Unimplemented");

		finder.matchAST(ast->getASTContext());

		for(auto nodes : matches) {
			// add locations to a vector
		}
	}
}
