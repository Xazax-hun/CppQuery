#ifndef __CPPHIGHLIGHTER_H__
#define __CPPHIGHLIGHTER_H__

#include <vector>

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class CppHighlighter : public QSyntaxHighlighter {
	Q_OBJECT

public:
	CppHighlighter(QTextDocument* parent = nullptr);

protected:
	void highlightBlock(const QString &text);

private:
	struct HighlightRule
	{
		QRegularExpression pattern;
		QTextCharFormat format;
	};

	std::vector<HighlightRule> highlightRules;

	QRegularExpression commentStartExpression;
	QRegularExpression commentEndExpression;

	QTextCharFormat keywordFormat;
	QTextCharFormat singleLineCommentFormat;
	QTextCharFormat multiLineCommentFormat;
	QTextCharFormat quotationFormat;
	QTextCharFormat sigilsFormat;
};

#endif
