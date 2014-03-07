#include "autocompleter_textedit.h"

#include <QCompleter>
#include <QtWidgets>

AutoCompleterTextEdit::AutoCompleterTextEdit(QWidget *parent)
    : QTextEdit(parent), comp(nullptr) {}

void AutoCompleterTextEdit::setCompleter(QCompleter *c) {
  if (comp)
    disconnect(c, nullptr, this, nullptr);

  comp = c;

  if (!comp)
    return;

  comp->setWidget(this);
  comp->setCompletionMode(QCompleter::PopupCompletion);
  comp->setCaseSensitivity(Qt::CaseSensitive);

  connect(comp, static_cast<void (QCompleter::*)(const QString &)>(
                    &QCompleter::activated),
          this, &AutoCompleterTextEdit::insertCompletion);
}

QCompleter *AutoCompleterTextEdit::completer() const { return comp; }

void AutoCompleterTextEdit::insertCompletion(const QString &completion) {
  if (comp->widget() != this)
    return;

  QTextCursor tc = textCursor();
  int extra = completion.length() - comp->completionPrefix().length();
  tc.movePosition(QTextCursor::Left);
  tc.movePosition(QTextCursor::EndOfWord);
  tc.insertText(completion.right(extra));
  setTextCursor(tc);
}

QString AutoCompleterTextEdit::textUnderCursor() const {
  QTextCursor tc = textCursor();
  tc.movePosition(QTextCursor::Left);
  tc.select(QTextCursor::WordUnderCursor);
  return tc.selectedText();
}

void AutoCompleterTextEdit::focusInEvent(QFocusEvent *e) {
  if (comp)
    comp->setWidget(this);

  QTextEdit::focusInEvent(e);
}

void AutoCompleterTextEdit::keyPressEvent(QKeyEvent *e) {
  // If the completer is visible it is responsible to handle the following
  // events
  if (comp && comp->popup()->isVisible()) {
    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
      e->ignore();
      return;
    default:
      break;
    }
  }

  QTextEdit::keyPressEvent(e);

  QString completionPrefix = textUnderCursor();

  if (e->text().isEmpty() || completionPrefix.length() < 3 ||
      e->text().right(1) == "(") {
    comp->popup()->hide();
    return;
  }

  if (completionPrefix != comp->completionPrefix()) {
    comp->setCompletionPrefix(completionPrefix);
    comp->popup()->setCurrentIndex(comp->completionModel()->index(0, 0));
  }

  QRect cr = cursorRect();
  cr.setWidth(comp->popup()->sizeHintForColumn(0) +
              comp->popup()->verticalScrollBar()->sizeHint().width());
  comp->complete(cr);
}
