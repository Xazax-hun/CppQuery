#include "querycatalog_window.h"

#include "querycatalog_model.h"

using namespace CppQuery;

QueryCatalogWindow::QueryCatalogWindow(QWidget *parent) : QDialog(parent) {
  resize(800, 600);
  setModal(true);

  queryTable = new QTableView(this);
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->addWidget(queryTable);
  setLayout(layout);

  queryTable->setEditTriggers(QAbstractItemView::SelectedClicked |
                              QAbstractItemView::AnyKeyPressed);

  connect(queryTable, &QAbstractItemView::doubleClicked,
          [this](const QModelIndex &index) {
    emit doubleClicked(index); hide();
  });
}

void QueryCatalogWindow::readFromFile(const std::string &file) {
  delete queryTable->model();
  queryTable->setModel(new QueryCatalogModel(file));
  queryTable->resizeColumnsToContents();

}

QueryCatalogWindow::~QueryCatalogWindow() { delete queryTable->model(); }


void QueryCatalogWindow::addQuery(const std::string& query) {
	QAbstractItemModel* model = queryTable->model();
	model->insertRows(model->rowCount(), 1);
	QModelIndex index = model->index(model->rowCount() - 1, 2);
	model->setData(index, QString::fromStdString(query));
	queryTable->setModel(model);
}
