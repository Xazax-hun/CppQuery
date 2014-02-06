#ifndef __CODEVIEW_WIDGET_H__
#define __CODEVIEW_WIDGET_H__

#include <QtWidgets>

// Inspired by QT examples

class CppHighlighter;
class LineNumberArea;

class CodeViewArea : public QPlainTextEdit
{
	Q_OBJECT

public:
	CodeViewArea(QWidget* parent = nullptr);

	void lineNumberAreaPaintEvent(QPaintEvent* event);
	int lineNumberAreaWidth();
	void highlightCurrentLine();

protected:
	void resizeEvent(QResizeEvent* event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect&, int);

private:
	QWidget* lineNumberArea;
	CppHighlighter* highlighter;
};


class LineNumberArea : public QWidget
{
public:
	LineNumberArea(CodeViewArea* codeViewArea) : QWidget(codeViewArea), codeViewArea(codeViewArea) {}

	QSize sizeHint() const {
		return QSize(codeViewArea->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent* event) {
		codeViewArea->lineNumberAreaPaintEvent(event);
	}

private:
	CodeViewArea* codeViewArea;
};

#endif
