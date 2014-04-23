#ifndef __SEARCHRESULTS_WIDGET_H__
#define __SEARCHRESULTS_WIDGET_H__

#include <QtWidgets>

namespace CppQuery {

/// \brief Search results widget.
///
/// This widget is responsible for displaying query results and filtering the
/// results based on the file path.
class SearchResults : public QWidget {
  Q_OBJECT
public:
  SearchResults(QWidget *parent);

  QAbstractItemModel *model();
  void setModel(QAbstractItemModel *model);

  void filterResults();

signals:
  void doubleClicked(const QModelIndex &index);

private:
  QTableView *searchResults;
  QLineEdit *filterText;
  QPushButton *filterButton;
  QSortFilterProxyModel *proxyModel;
};
}

#endif