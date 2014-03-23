#include "query_completer.h"

#include "automata_generator/matcher_traits.hpp"
#include "automata_generator/automata_generator.hpp"

namespace {
using namespace clang::ast_matchers;
typedef typename Automata<
    MATCHER(ctorInitializer), MATCHER(accessSpecDecl),
    MATCHER(classTemplateDecl), MATCHER(classTemplateSpecializationDecl),
    MATCHER(constructorDecl), MATCHER(decl), MATCHER(declaratorDecl),
    MATCHER(destructorDecl), MATCHER(enumConstantDecl), MATCHER(enumDecl),
    MATCHER(fieldDecl), MATCHER(friendDecl), MATCHER(functionDecl),
    MATCHER(functionTemplateDecl), MATCHER(methodDecl), MATCHER(namedDecl),
    MATCHER(namespaceDecl), MATCHER(parmVarDecl), MATCHER(recordDecl),
    MATCHER(unresolvedUsingValueDecl), MATCHER(usingDecl), MATCHER(varDecl),
    MATCHER(nestedNameSpecifierLoc), MATCHER(nestedNameSpecifier),
    MATCHER(qualType), MATCHER(arraySubscriptExpr), MATCHER(asmStmt),
    MATCHER(binaryOperator), MATCHER(bindTemporaryExpr), MATCHER(boolLiteral),
    MATCHER(breakStmt), MATCHER(cStyleCastExpr), MATCHER(callExpr),
    MATCHER(caseStmt), MATCHER(castExpr), MATCHER(catchStmt),
    MATCHER(characterLiteral), MATCHER(compoundLiteralExpr),
    MATCHER(compoundStmt), MATCHER(conditionalOperator), MATCHER(constCastExpr),
    MATCHER(constructExpr), MATCHER(continueStmt), MATCHER(declRefExpr),
    MATCHER(declStmt), MATCHER(defaultArgExpr), MATCHER(defaultStmt),
    MATCHER(deleteExpr), MATCHER(doStmt), MATCHER(dynamicCastExpr),
    MATCHER(explicitCastExpr), MATCHER(expr), MATCHER(floatLiteral),
    MATCHER(forRangeStmt), MATCHER(forStmt), MATCHER(functionalCastExpr),
    MATCHER(gotoStmt), MATCHER(ifStmt), MATCHER(implicitCastExpr),
    MATCHER(initListExpr), MATCHER(integerLiteral), MATCHER(labelStmt),
    MATCHER(lambdaExpr), MATCHER(materializeTemporaryExpr),
    MATCHER(memberCallExpr), MATCHER(memberExpr), MATCHER(newExpr),
    MATCHER(nullPtrLiteralExpr), MATCHER(nullStmt), MATCHER(operatorCallExpr),
    MATCHER(reinterpretCastExpr), MATCHER(returnStmt), MATCHER(staticCastExpr),
    MATCHER(stmt), MATCHER(stringLiteral), MATCHER(switchCase),
    MATCHER(switchStmt), MATCHER(temporaryObjectExpr), MATCHER(thisExpr),
    MATCHER(throwExpr), MATCHER(tryStmt), MATCHER(unaryExprOrTypeTraitExpr),
    MATCHER(unaryOperator), MATCHER(unresolvedConstructExpr),
    MATCHER(userDefinedLiteral), MATCHER(whileStmt), MATCHER(typeLoc),
    MATCHER(arrayType), MATCHER(atomicType), MATCHER(autoType),
    MATCHER(blockPointerType), MATCHER(builtinType), MATCHER(complexType),
    MATCHER(constantArrayType), MATCHER(dependentSizedArrayType),
    MATCHER(elaboratedType), MATCHER(functionType),
    MATCHER(incompleteArrayType), MATCHER(lValueReferenceType),
    MATCHER(memberPointerType), MATCHER(parenType), MATCHER(pointerType),
    MATCHER(rValueReferenceType), MATCHER(recordType), MATCHER(referenceType),
    MATCHER(templateSpecializationType), MATCHER(type), MATCHER(typedefType),
    MATCHER(unaryTransformType),
    MATCHER(variableArrayType)>::result GeneratedAutomata;
}

using namespace CppQuery;

QueryCompleter::QueryCompleter(const QStringList &words, QObject *parent)
    : QCompleter(parent), list(words) {}

void QueryCompleter::updateModelFromCtxt(const QString &text) {}