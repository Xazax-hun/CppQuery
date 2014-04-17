#include "querycatalog_window.h"

#include "querycatalog_model.h"

using namespace CppQuery;

QueryCatalogWindow::QueryCatalogWindow(const std::string &file,
                                       QWidget *parent)
    : QMainWindow(parent) {

  resize(800, 600);

  queryTable = new QTableView(this);
  queryTable->setModel(new QueryCatalogModel(file));
  setCentralWidget(queryTable);
  queryTable->resizeColumnsToContents();
}

QueryCatalogWindow::~QueryCatalogWindow() { delete queryTable->model(); }
