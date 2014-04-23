#ifndef __QUERYCATALOG_MODEL_H__
#define __QUERYCATALOG_MODEL_H__

#include <string>

#include <QAbstractTableModel>

#include "util.h"
#include "query_catalog.h"

namespace CppQuery {

/// \brief Query Catalog Model class.
///
/// This class determines how the entries that are in the Query Catalog should
/// be displayed and how can the entries be modified. This class also
/// responsible for adding and removing entries from the Query Catalog. It is
/// using the QueryCatalogParser class to read and write entries from and to
/// the Query Catalog.
class QueryCatalogModel : public QAbstractTableModel {
  Q_OBJECT

public:
  QueryCatalogModel(const std::string &file);
  ~QueryCatalogModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;

  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole);
  Qt::ItemFlags flags(const QModelIndex &index) const;

  bool insertRows(int row, int count,
                  const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex());

private:
  OwningPtr<CppQuery::QueryCatalogParser> parser;
  std::vector<CppQuery::CatalogEntry> entries;
};
}

#endif