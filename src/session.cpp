#include "session.h"

#include <cassert>

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "clang/AST/AST.h"

using namespace clang;
using namespace tooling;

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

void Session::runQuery(const std::string& query)
{
}
