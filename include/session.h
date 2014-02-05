#ifndef __SESSION_H__
#define __SESSION_H__

#include <set>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "clang/Basic/SourceLocation.h"

namespace clang {
	class ASTUnit;

	namespace tooling {
		class JSONCompilationDatabase;
		class ClangTool;
	}
}

class Session {
public:
	Session(const std::string& databasePath);
	~Session();

	void runQuery(const std::string& query);

	std::set<std::string> getMatches();

private:
	std::unique_ptr<clang::tooling::JSONCompilationDatabase> compilationDatabase;
 	std::vector<clang::ASTUnit *> ASTlist;
 	std::vector<std::string> files;
 	std::unique_ptr<clang::tooling::ClangTool> tool;
 	std::set<std::string> foundMatches;
};

#endif
