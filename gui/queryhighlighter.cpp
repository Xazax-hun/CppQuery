#include "queryhighlighter.h"

QueryHighlighter::QueryHighlighter(QTextDocument* parent)
	: QSyntaxHighlighter(parent) {

	HighlightRule rule;

	// Keywords
	keywordFormat.setForeground(Qt::darkBlue);
	keywordFormat.setFontWeight(QFont::Bold);

	QStringList keywords;
	keywords <<
		#include "matchers.txt"
	;

	for (const QString& pattern: keywords) {
		rule.pattern = QRegularExpression(pattern);
		rule.format = keywordFormat;
		highlightRules.push_back(rule);
	}

	// Sigils
	sigilsFormat.setForeground(Qt::red);
	sigilsFormat.setFontWeight(QFont::Bold);

	QStringList sigils;
	sigils << "\\(" << "\\)";

	for (const QString& pattern: sigils) {
		rule.pattern = QRegularExpression(pattern);
		rule.format = sigilsFormat;
		highlightRules.push_back(rule);
	}

	// String literals
	quotationFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegularExpression("\"(\\.|[^\"])*\"");
	rule.format = quotationFormat;
	highlightRules.push_back(rule);

}

void QueryHighlighter::highlightBlock(const QString& text) {

	for (const HighlightRule &rule: highlightRules) {
		const QRegularExpression& expression(rule.pattern);
		QRegularExpressionMatch m = expression.match(text);
		while (m.hasMatch()) {
			int index = m.capturedStart();
			int length = m.capturedLength();
			setFormat(index, length, rule.format);
			m = expression.match(text, index + length);
		}
	}
}
