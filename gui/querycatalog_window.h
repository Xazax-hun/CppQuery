#ifndef __QUERYCATALOG_WINDOW_H__
#define __QUERYCATALOG_WINDOW_H__

#include <QtWidgets>
#include <string>

namespace CppQuery {

/// \brief Query Catalog dialog.
///
/// This class displays the Query Catalog in a way that is determined by the
/// QueryCatalogModel class. 
class QueryCatalogWindow : public QDialog {
  Q_OBJECT

public:
  QueryCatalogWindow(QWidget *parent);
  ~QueryCatalogWindow();

  void readFromFile(const std::string &file);

  void addQuery(const std::string& query);

signals:
  void doubleClicked(const QModelIndex &index);

private:
  QTableView *queryTable;
};
}

#endif