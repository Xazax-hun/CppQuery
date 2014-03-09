#ifndef __QUERYHIGHLIGHTER_H__
#define __QUERYHIGHLIGHTER_H__

#include <vector>

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

namespace CppQuery {

/// \brief Syntax highlighting of ASTMatcher queries.
class QueryHighlighter : public QSyntaxHighlighter {
  Q_OBJECT

public:
  QueryHighlighter(QTextDocument *parent = nullptr);

protected:
  void highlightBlock(const QString &text);

private:
  struct HighlightRule {
    QRegularExpression pattern;
    QTextCharFormat format;
  };

  std::vector<HighlightRule> highlightRules;

  QTextCharFormat keywordFormat;
  QTextCharFormat quotationFormat;
  QTextCharFormat sigilsFormat;
};
}

#endif
