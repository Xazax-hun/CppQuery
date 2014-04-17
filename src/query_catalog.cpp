#include "query_catalog.h"

#include <fstream>
#include <sstream>

using namespace CppQuery;

bool QueryCatalogParser::parseFile() {
  entries.clear();

  std::ifstream input(file);

  if (!input)
    return false;

  std::string line;
  while (getline(input, line)) {
    std::stringstream ss(line);

    CatalogEntry c;

    getline(ss, c.name, '|');
    getline(ss, c.description, '|');
    getline(ss, c.query, '|');

    entries.push_back(c);
  }

  return true;
}

bool QueryCatalogParser::saveFile() {
  std::ofstream output(file);

  if (!output)
    return false;

  for (auto &entry : entries) {
    output << entry.name << "|" << entry.description << "|" << entry.query
           << std::endl;
  }

  return true;
}

const std::vector<CatalogEntry>& QueryCatalogParser::getEntries() const { return entries; }

void QueryCatalogParser::setEntries(const std::vector<CatalogEntry> &entries) {
  this->entries = entries;
}