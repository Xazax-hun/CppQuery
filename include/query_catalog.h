#ifndef __QUERY_CATALOG_H__
#define __QUERY_CATALOG_H__

#include <string>
#include <vector>

namespace CppQuery {

/// Small triplet to store name, description and query text in the query
/// catalog.
struct CatalogEntry {
  CatalogEntry() {}
  std::string name = "", description = "", query = "";
};

/// \brief Parse query catalog files.
///
/// This class can parse from and serialize to query catalog files. Those files
/// are simple text files containing CatalogEntry objects.
class QueryCatalogParser {
public:
  /// The file that is the source of the query catalog entries
  QueryCatalogParser(const std::string &file) : file(file) {}

  /// Read the catalog entries from the file specified in the constructor call
  bool parseFile();

  /// Write catalog entries to the file specified in the constructor call
  bool saveFile();

  const std::vector<CatalogEntry> &getEntries() const;
  void setEntries(const std::vector<CatalogEntry> &entries);

private:
  std::string file;
  std::vector<CatalogEntry> entries;
};
}

#endif