#include "searchresults_widget.h"

using namespace CppQuery;

SearchResults::SearchResults(QWidget *parent) : QWidget(parent) {
  filterText = new QLineEdit(this);
  filterButton = new QPushButton(tr("Filter"), this);
  searchResults = new QTableView(this);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(searchResults);

  QHBoxLayout *buttonLayout = new QHBoxLayout();

  layout->addLayout(buttonLayout);

  buttonLayout->addWidget(filterText);
  buttonLayout->addWidget(filterButton);

  searchResults->setSelectionMode(QAbstractItemView::NoSelection);
  QStandardItemModel *model = new QStandardItemModel(0, 6, searchResults);
  QStringList headers;
  headers << tr("id") << tr("Filename") << tr("Start Line") << tr("Start Col")
          << tr("End Line") << tr("End Col");
  model->setHorizontalHeaderLabels(headers);

  proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(model);
  proxyModel->setFilterKeyColumn(1);
  proxyModel->setFilterRegExp(
      QRegExp("*", Qt::CaseInsensitive, QRegExp::Wildcard));

  searchResults->setModel(proxyModel);
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

  connect(filterButton, &QPushButton::clicked, this,
          &SearchResults::filterResults);
  connect(filterText, &QLineEdit::returnPressed, this,
          &SearchResults::filterResults);
}

QAbstractItemModel *SearchResults::model() { return searchResults->model(); }

void SearchResults::setModel(QAbstractItemModel *model) {
  searchResults->setModel(model);
}

void SearchResults::filterResults() {
  QString text = filterText->text();
  if (text != "") {
    proxyModel->setFilterRegExp(
        QRegExp(text, Qt::CaseInsensitive, QRegExp::Wildcard));
  } else {
    proxyModel->setFilterRegExp(
        QRegExp("*", Qt::CaseInsensitive, QRegExp::Wildcard));
  }
}