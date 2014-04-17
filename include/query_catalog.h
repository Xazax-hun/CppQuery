#ifndef __QUERY_CATALOG_H__
#define __QUERY_CATALOG_H__

#include <string>
#include <vector>

namespace CppQuery {

struct CatalogEntry {
  CatalogEntry() {}
  std::string name = "", description = "", query = "";
};

class QueryCatalogParser {
public:
  QueryCatalogParser(const std::string &file) : file(file) {}

  bool parseFile();
  bool saveFile();

  const std::vector<CatalogEntry>& getEntries() const;
  void setEntries(const std::vector<CatalogEntry> &entries);

private:
  std::string file;
  std::vector<CatalogEntry> entries;
};
}

#endif