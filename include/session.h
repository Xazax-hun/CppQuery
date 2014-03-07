#ifndef __SESSION_H__
#define __SESSION_H__

#include <set>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <exception>

#include "clang/Basic/SourceLocation.h"

namespace clang {
class ASTUnit;

namespace tooling {
class JSONCompilationDatabase;
class ClangTool;
}
}

/// Exception when the compilation database is invalid or non-existent. The
/// reason contains the detailed error report.
class DatabaseError : std::exception {
public:
  DatabaseError(const std::string &error) : reason(error) {}
  const std::string &getReason() const { return reason; }
  const char *what() const noexcept override {
    return "Unable to load compilation database.";
  }

private:
  std::string reason;
};

/// Exception for syntactic and semantic errors of queries. The reason contains
/// the detailed error report.
class QueryError : std::exception {
public:
  QueryError(const std::string &error) : reason(error) {}
  const std::string &getReason() const { return reason; }
  const char *what() const noexcept override {
    return "Unable to parse the query.";
  }

private:
  std::string reason;
};

/// Small POD type to represent a single query result.
struct Match {
  std::string fileName, id;
  unsigned startLine, startCol;
  unsigned endLine, endCol;
};

bool operator<(const Match &, const Match &);

/// \brief Storage and execution of query related parts.
///
/// This class stores the compilation database, the AST of the translation units
/// and the list of files. It also contains the results form a query. A session
/// represents a project's set of ASTs. It should be in the memory as long as
/// the project is inspected. When a project is closed and a new one is opened,
/// it is safe to delete the old session.
class Session {
public:
  Session(const std::string &databasePath);
  ~Session();

  unsigned getFileCount() { return files.size(); }
  void parseFiles(const std::function<bool(const std::string &)> &onTUend);

  void runQuery(const std::string &query);

  const std::set<Match> &getMatches() const;

private:
  std::unique_ptr<clang::tooling::JSONCompilationDatabase> compilationDatabase;
  std::vector<clang::ASTUnit *> ASTlist;
  std::vector<std::string> files;
  std::unique_ptr<clang::tooling::ClangTool> tool;
  std::set<Match> foundMatches;
};

#endif
