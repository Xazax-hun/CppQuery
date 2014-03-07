#ifndef __AUTOCOMPLETER_TEXTEDIT_H__
#define __AUTOCOMPLETER_TEXTEDIT_H__

#include <QTextEdit>

class QCompleter;

class AutoCompleterTextEdit : public QTextEdit {
  Q_OBJECT

public:
  AutoCompleterTextEdit(QWidget *parent = nullptr);

  void setCompleter(QCompleter *c);
  QCompleter *completer() const;

protected:
  void keyPressEvent(QKeyEvent *e);
  void focusInEvent(QFocusEvent *e);

private
slots:
  void insertCompletion(const QString &completion);

private:
  QString textUnderCursor() const;

private:
  QCompleter *comp;
};

#endif
