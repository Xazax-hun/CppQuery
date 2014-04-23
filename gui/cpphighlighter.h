#ifndef __CPPHIGHLIGHTER_H__
#define __CPPHIGHLIGHTER_H__

#include <vector>

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

namespace CppQuery {

/// \brief Highlighting C++ source files.
///
/// This class provides basic syntax highlighting for C++ source files. This
/// does not include semantic highlighting.
class CppHighlighter : public QSyntaxHighlighter {
  Q_OBJECT

public:
  CppHighlighter(QTextDocument *parent = nullptr);

protected:
  void highlightBlock(const QString &text);

private:
  /// Helper structure for pairing patterns with color formats.
  struct HighlightRule {
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
  QTextCharFormat directiveFormat;
};
}

#endif
