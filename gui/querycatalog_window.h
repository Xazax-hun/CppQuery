#ifndef __QUERYCATALOG_WINDOW_H__
#define __QUERYCATALOG_WINDOW_H__

#include <QtWidgets>
#include <string>

namespace CppQuery {

class QueryCatalogWindow : public QMainWindow {
  Q_OBJECT

public:
  QueryCatalogWindow(const std::string &file, QWidget *parent);
  ~QueryCatalogWindow();

  void addQuery(const std::string& query);

private:
  QTableView *queryTable;
};
}

#endif