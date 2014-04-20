#ifndef __FILESELECTOR_WINDOW_H__
#define __FILESELECTOR_WINDOW_H__

#include <QtWidgets>
#include <QStringList>

namespace CppQuery {

class FileSelectorDialog : public QDialog {
  Q_OBJECT

public:
  FileSelectorDialog(const QStringList &list, QWidget *parent);

  void selectAll();
  void deselectAll();
  void okClicked();

  const QStringList &getSelectedFiles() const { return selectedFiles; }

private:
  QStringList selectedFiles;
  QListWidget *listWidget;
  QPushButton *selectAllButton;
  QPushButton *deselectAllButton;
  QPushButton *okButton;
};
}

#endif
