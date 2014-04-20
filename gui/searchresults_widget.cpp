#include "searchresults_widget.h"

using namespace CppQuery;

SearchResults::SearchResults(QWidget *parent) : QWidget(parent) {
  filterText = new QLineEdit(this);
  filterButton = new QPushButton(tr("Filter"), this);
  searchResults = new QTableView(this);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(searchResults);

  QHBoxLayout *buttonLayout = new QHBoxLayout(this);

  layout->addLayout(buttonLayout);

  buttonLayout->addWidget(filterText);
  buttonLayout->addWidget(filterButton);

  searchResults->setSelectionMode(QAbstractItemView::NoSelection);
  QStandardItemModel *model = new QStandardItemModel(0, 6, searchResults);
  QStringList headers;
  headers << tr("id") << tr("Filename") << tr("Start Line") << tr("Start Col")
          << tr("End Line") << tr("End Col");
  model->setHorizontalHeaderLabels(headers);

  searchResults->setModel(model);
  searchResults->setGridStyle(Qt::NoPen);
  searchResults->setAlternatingRowColors(true);
  searchResults->verticalHeader()->hide();
  searchResults->setEditTriggers(QAbstractItemView::NoEditTriggers);
  searchResults->setSelectionMode(QAbstractItemView::NoSelection);
  searchResults->setDragEnabled(false);
  searchResults->setTextElideMode(Qt::ElideLeft);
  searchResults->resizeColumnsToContents();
  searchResults->horizontalHeader()->setStretchLastSection(true);

  connect(searchResults, &QTableView::doubleClicked,
          [this](const QModelIndex &index) { emit doubleClicked(index); });
}

QAbstractItemModel *SearchResults::model() { return searchResults->model(); }

void SearchResults::setModel(QAbstractItemModel *model) {
  searchResults->setModel(model);
}
