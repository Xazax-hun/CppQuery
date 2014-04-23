#ifndef __FILESELECTOR_WINDOW_H__
#define __FILESELECTOR_WINDOW_H__

#include <QtWidgets>
#include <QStringList>

namespace CppQuery {

/// \brief Select list of files to parse.
///
/// This class represents a dialog that is used to select which files of a
/// compilation database should be parsed.
class FileSelectorDialog : public QDialog {
  Q_OBJECT

public:
  /// The consturctor expects the list of files that are in the compilation
  /// database. The paths must be absolute paths.
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
