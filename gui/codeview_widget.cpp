#include "codeview_widget.h"

#include <algorithm>
#include <QDebug>

#include "cpphighlighter.h"

CodeViewArea::CodeViewArea(QWidget *parent) : QPlainTextEdit(parent) {
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(12);

  setFont(font);

  highlighter = new CppHighlighter(document());

  lineNumberArea = new LineNumberArea(this);

  connect(this, &QPlainTextEdit::blockCountChanged, this,
          &CodeViewArea::updateLineNumberAreaWidth);
  connect(this, &QPlainTextEdit::updateRequest, this,
          &CodeViewArea::updateLineNumberArea);

  updateLineNumberAreaWidth(0);
}

int CodeViewArea::lineNumberAreaWidth() {
  int digits = 1;
  int max = std::max(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

  return space;
}

void CodeViewArea::updateLineNumberAreaWidth(int) {
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeViewArea::updateLineNumberArea(const QRect &rect, int dy) {
  if (dy)
    lineNumberArea->scroll(0, dy);
  else
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

  if (rect.contains(viewport()->rect()))
    updateLineNumberAreaWidth(0);
}

void CodeViewArea::resizeEvent(QResizeEvent *e) {
  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  lineNumberArea->setGeometry(
      QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeViewArea::lineNumberAreaPaintEvent(QPaintEvent *event) {
  QPainter painter(lineNumberArea);
  painter.fillRect(event->rect(), Qt::lightGray);

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int)blockBoundingRect(block).height();

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::black);
      painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                       Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int)blockBoundingRect(block).height();
    ++blockNumber;
  }
}

void CodeViewArea::highlightArea(int startLine, int startCol, int endLine,
                                 int endCol) {
  QList<QTextEdit::ExtraSelection> extraSelections;
  QTextEdit::ExtraSelection selection;

  QColor selectionColor = QColor(Qt::yellow).lighter(160);
  selection.format.setBackground(selectionColor);

  QTextCursor cursor = textCursor();
  cursor.clearSelection();
  cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
  cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor,
                      startLine - 1);
  cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor,
                      startCol - 1);

  setTextCursor(cursor);

  // selection
  cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor,
                      endLine - startLine);
  cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,
                      endCol);

  selection.cursor = cursor;
  extraSelections.append(selection);
  setExtraSelections(extraSelections);
}
