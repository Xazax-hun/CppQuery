#ifndef __CODEVIEW_WIDGET_H__
#define __CODEVIEW_WIDGET_H__

#include <QtWidgets>

namespace CppQuery {

class CppHighlighter;
class LineNumberArea;

/// \brief The C++ source code viewer.
///
/// This class provides several advanced features over QPlainTextEdit. This
/// includes syntax highlighting of C/C++ files, highlighting arbitrary area of
/// the text and displaying line numbers at the right side of the widget.
/// This class was inspired bz Qt Examples
class CodeViewArea : public QPlainTextEdit {
  Q_OBJECT

public:
  CodeViewArea(QWidget *parent = nullptr);

  void lineNumberAreaPaintEvent(QPaintEvent *event);
  int lineNumberAreaWidth();
  void highlightArea(int startLine, int startCol, int endLine, int endCol);

protected:
  void resizeEvent(QResizeEvent *event);

private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void updateLineNumberArea(const QRect &, int);

private:
  QWidget *lineNumberArea;
  CppHighlighter *highlighter;
};

/// \brief Line numbers at the right side of a text widget.
///
/// This class is repsonsible for displaying the line numbers for a text editor
/// or viewer. It is used by CodeViewArea.
class LineNumberArea : public QWidget {
public:
  LineNumberArea(CodeViewArea *codeViewArea)
      : QWidget(codeViewArea), codeViewArea(codeViewArea) {}

  QSize sizeHint() const {
    return QSize(codeViewArea->lineNumberAreaWidth(), 0);
  }

protected:
  void paintEvent(QPaintEvent *event) {
    codeViewArea->lineNumberAreaPaintEvent(event);
  }

private:
  CodeViewArea *codeViewArea;
};
}

#endif
