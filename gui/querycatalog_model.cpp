#include "querycatalog_model.h"

#include <cassert>

#include "query_catalog.h"

using namespace CppQuery;

QueryCatalogModel::QueryCatalogModel(const std::string &file) {
  parser.reset(new QueryCatalogParser(file));
  bool b = parser->parseFile();
  assert(b);

  entries = parser->getEntries();
}

QueryCatalogModel::~QueryCatalogModel() {
  if (parser) {
    parser->setEntries(entries);
    parser->saveFile();
  }
}

int QueryCatalogModel::rowCount(const QModelIndex &parent) const {
  return entries.size();
}

int QueryCatalogModel::columnCount(const QModelIndex &parent) const {
  return 3;
}

QVariant QueryCatalogModel::data(const QModelIndex &index, int role) const {
  if (role != Qt::DisplayRole && role != Qt::EditRole)
    return QVariant();

  std::string data;
  switch (index.column()) {
  case 0:
    data = entries[index.row()].name;
    break;
  case 1:
    data = entries[index.row()].description;
    break;
  case 2:
    data = entries[index.row()].query;
    break;
  }

  return QString::fromStdString(data);
}

QVariant QueryCatalogModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const {

  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Vertical)
    return QVariant();

  switch (section) {
  case 0:
    return QString(tr("Name"));
  case 1:
    return QString(tr("Description"));
  case 2:
    return QString(tr("Query"));
  }

  return QVariant();
}

bool QueryCatalogModel::setData(const QModelIndex &index, const QVariant &value,
                                int role) {
  switch (index.column()) {
  case 0:
    entries[index.row()].name = value.toString().toStdString();
    break;
  case 1:
    entries[index.row()].description = value.toString().toStdString();
    break;
  case 2:
    entries[index.row()].query = value.toString().toStdString();
    break;
  }

  return true;
}

Qt::ItemFlags QueryCatalogModel::flags(const QModelIndex &index) const {
  return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool QueryCatalogModel::insertRows(int row, int count,
                                   const QModelIndex &parent) {
  beginInsertRows(parent, row, row + count - 1);
  entries.insert(entries.begin() + row, count, CatalogEntry{});
  endInsertRows();
  return true;
}

bool QueryCatalogModel::removeRows(int row, int count,
                                   const QModelIndex &parent) {
  beginRemoveRows(parent, row, row + count - 1);
  entries.erase(entries.begin() + row, entries.begin() + row + count);
  endRemoveRows();
  return true;
}
