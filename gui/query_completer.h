#ifndef __QUERY_COMPLETER_H__
#define __QUERY_COMPLETER_H__

#include <QCompleter>
#include <QStringList>
#include <QStringListModel>
#include <QString>

namespace CppQuery {

class QueryCompleter : public QCompleter {
  Q_OBJECT
public:
  QueryCompleter(QObject *parent);

  void updateModelFromCtxt(const QString &text, const QString &prefix);

private:
  QStringList list;
  QStringList allMatchers;
  QStringListModel model;
};
}

#endif