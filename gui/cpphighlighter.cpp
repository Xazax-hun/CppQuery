#include "cpphighlighter.h"

CppHighlighter::CppHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {

  HighlightRule rule;

  // Keywords
  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);

  QStringList keywords;
  keywords << R"(\balignas\b)" << R"(\balignof\b)" << R"(\band\b)" <<
      R"(\band_eq\b)" << R"(\basm\b)" << R"(\bauto\b)" << R"(\bbitand\b)" <<
      R"(\bbitor\b)" << R"(\bbool\b)" << R"(\bbreak\b)" << R"(\bcase\b)"
           << R"(\bcatch\b)" << R"(\bchar\b)" << R"(\bchar16_t\b)" <<
      R"(\bchar32_t\b)" << R"(\bclass\b)" << R"(\bcompl\b)" << R"(\bconst\b)"
           << R"(\bconstexpr\b)" << R"(\bconst_cast\b)" << R"(\bcontinue\b)"
           << R"(\bdecltype\b)" << R"(\bdefault\b)" << R"(\bdelete\b)" <<
      R"(\bdo\b)" << R"(\bdouble\b)" << R"(\bdynamic_cast\b)" << R"(\belse\b)"
           << R"(\benum\b)" << R"(\bexplicit\b)" << R"(\bexport\b)" <<
      R"(\bextern\b)" << R"(\bfalse\b)" << R"(\bfinal\b)" << R"(\bfloat\b)"
           << R"(\bfor\b)" << R"(\bfriend\b)" << R"(\bgoto\b)" <<
      R"(\bif\b)" << R"(\binline\b)" << R"(\bint\b)" << R"(\blong\b)"
           << R"(\bmutable\b)" << R"(\bnamespace\b)" << R"(\bnew\b)" <<
      R"(\bnoexcept\b)" << R"(\bnot\b)" << R"(\bnot_eq\b)" << R"(\bnullptr\b)"
           << R"(\boperator\b)" << R"(\bor\b)" << R"(\bor_eq\b)" <<
      R"(\boverride\b)" << R"(\bprivate\b)" << R"(\bprotected\b)" <<
      R"(\bpublic\b)" << R"(\bregister\b)" << R"(\breinterpret_cast\b)"
           << R"(\breturn\b)" << R"(\bshort\b)" << R"(\bsigned\b)"
           << R"(\bsizeof\b)" << R"(\bstatic\b)" << R"(\bstatic_assert\b)"
           << R"(\bstatic_cast\b)" << R"(\bstruct\b)" << R"(\bswitch\b)" <<
      R"(\btemplate\b)" << R"(\bthis\b)" << R"(\bthread_local\b)" <<
      R"(\bthrow\b)" << R"(\btrue\b)" << R"(\btry\b)" << R"(\btypedef\b)"
           << R"(\btypeid\b)" << R"(\btypename\b)" << R"(\bunion\b)" <<
      R"(\bunsigned\b)" << R"(\busing\b)" << R"(\bvirtual\b)" << R"(\bvoid\b)"
           << R"(\bvolatile\b)" << R"(\bwchar_t\b)" << R"(\bwhile\b)" <<
      R"(\bxor\b)" << R"(\bxor_eq \b)" << R"(\bslots\b)" << R"(\bsignals\b)";

  for (const QString &pattern : keywords) {
    rule.pattern = QRegularExpression(pattern);
    rule.format = keywordFormat;
    highlightRules.push_back(rule);
  }

  // Sigils
  sigilsFormat.setForeground(Qt::red);
  sigilsFormat.setFontWeight(QFont::Bold);

  QStringList sigils;
  sigils << "\\(" << "\\)" << "\\[" << "\\]" << "&" << "\\|"
         << "\\<" << "\\>" << "!" << ";" << "\\{" << "\\}"
         << "\\?" << ":" << "\\+" << "\\*" << "-" << "/"
         << "\\^" << "=" << "~" << "%";

  for (const QString &pattern : sigils) {
    rule.pattern = QRegularExpression(pattern);
    rule.format = sigilsFormat;
    highlightRules.push_back(rule);
  }

  // Comments
  singleLineCommentFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegularExpression("//[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightRules.push_back(rule);

  multiLineCommentFormat.setForeground(Qt::darkGreen);

  commentStartExpression = QRegularExpression("/\\*");
  commentEndExpression = QRegularExpression("\\*/");

  // String literals
  quotationFormat.setForeground(Qt::gray);
  rule.pattern = QRegularExpression("\"(\\.|[^\"])*\"");
  rule.format = quotationFormat;
  highlightRules.push_back(rule);

  // Directives
  directiveFormat.setForeground(Qt::darkGray);
  rule.pattern = QRegularExpression("#[^\n]*");
  rule.format = directiveFormat;
  highlightRules.push_back(rule);
}

void CppHighlighter::highlightBlock(const QString &text) {

  for (const HighlightRule &rule : highlightRules) {
    const QRegularExpression &expression(rule.pattern);
    QRegularExpressionMatch m = expression.match(text);
    while (m.hasMatch()) {
      int index = m.capturedStart();
      int length = m.capturedLength();
      setFormat(index, length, rule.format);
      m = expression.match(text, index + length);
    }
  }

  // Multiline comments
  setCurrentBlockState(0);

  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = commentStartExpression.match(text).capturedStart();

  while (startIndex >= 0) {
    QRegularExpressionMatch m = commentEndExpression.match(text, startIndex);
    int endIndex = m.capturedStart();
    int commentLength;
    if (endIndex == -1) {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    } else {
      commentLength = endIndex - startIndex + m.capturedLength();
    }
    setFormat(startIndex, commentLength, multiLineCommentFormat);
    startIndex = commentStartExpression.match(text, startIndex + commentLength)
                     .capturedStart();
  }
}
