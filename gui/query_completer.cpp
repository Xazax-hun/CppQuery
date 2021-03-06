#include "query_completer.h"

#include <algorithm>

#include "automata_generator/matcher_traits.hpp"
#include "automata_generator/automata_generator.hpp"

namespace {
using namespace clang::ast_matchers;

typedef typename Automata<
    // Node Matchers
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
    MATCHER(unaryTransformType), MATCHER(variableArrayType),

    // Narrowing matchers
    MATCHER(allOf), MATCHER(anyOf),
    MATCHER(anything), /*MATCHER(unless), Templated function on argument
                          (replace template parameter with dyntypedmatcher?)*/
    MATCHER(hasOperatorName), MATCHER(argumentCountIs), MATCHER(isImplicit),
    MATCHER(isWritten), MATCHER(hasOverloadedOperatorName), MATCHER(isConst),
    MATCHER(isOverride), MATCHER(isVirtual),
    MATCHER2(isDerivedFrom), MATCHER(isExplicitTemplateSpecialization),
    MATCHER2(isSameOrDerivedFrom), MATCHER(isTemplateInstantiation),
    MATCHER(statementCountIs), MATCHER(hasSize), MATCHER(declCountIs),
    MATCHER(equalsBoundNode), /* MATCHER(equalsNode),*/ MATCHER(isPrivate),
    MATCHER(isProtected), MATCHER(isPublic), MATCHER(isDefinition),
    MATCHER(isExternC), MATCHER(hasName), MATCHER(matchesName), MATCHER(asString),
    MATCHER(hasLocalQualifiers), MATCHER(isConstQualified), MATCHER(isInteger),
    MATCHER(ofKind), MATCHER(templateSpecializationType), MATCHER(type),
    MATCHER(typedefType), MATCHER(unaryTransformType),
    MATCHER(variableArrayType),

    // Traversal matchers
    MATCHER(forEachDescendant), MATCHER(forEach), MATCHER(hasAncestor),
    MATCHER(hasDescendant), MATCHER(has), MATCHER(hasParent), MATCHER(hasBase),
    MATCHER(hasIndex), MATCHER(hasElementType), MATCHER(hasValueType),
    MATCHER(hasDeducedType), MATCHER(hasEitherOperand), MATCHER(hasLHS),
    MATCHER(hasRHS), MATCHER(pointee), MATCHER(hasAnyArgument),
    MATCHER(hasArgument), MATCHER(hasDeclaration),
    MATCHER(forEachConstructorInitializer),
    MATCHER(hasAnyConstructorInitializer), MATCHER(forField),
    MATCHER(withInitializer), MATCHER(onImplicitObjectArgument),
    MATCHER(on),                          MATCHER2(thisPointerType),
    MATCHER(ofClass), MATCHER(hasMethod), MATCHER2(callee),
    MATCHER(hasCaseConstant), MATCHER(hasSourceExpression),
    MATCHER(hasAnyTemplateArgument), MATCHER(hasTemplateArgument),
    MATCHER(hasAnySubstatement), MATCHER(hasCondition),
    MATCHER(hasFalseExpression), MATCHER(hasTrueExpression),
    MATCHER(throughUsingDecl), MATCHER(to), MATCHER(containsDeclaration),
    MATCHER(hasSingleDecl), MATCHER(hasTypeLoc), MATCHER(hasDeclContext),
    MATCHER(hasBody), MATCHER(hasQualifier), MATCHER(namesType),
    MATCHER(hasDestinationType),
    MATCHER2(hasType), MATCHER(ignoringImpCasts),
    MATCHER(ignoringParenCasts), MATCHER(ignoringParenImpCasts),
    MATCHER(hasIncrement), MATCHER(hasLoopInit), MATCHER(hasAnyParameter),
    MATCHER(hasParameter), MATCHER(returns),
    MATCHER(hasConditionVariableStatement), MATCHER(hasImplicitDestinationType),
    MATCHER(hasObjectExpression), MATCHER(member),
    MATCHER2(hasPrefix), MATCHER(specifiesTypeLoc),
    MATCHER(specifiesNamespace), MATCHER(specifiesType), MATCHER(innerType),
    MATCHER(hasCanonicalType), MATCHER2(pointsTo), MATCHER2(references),
    MATCHER(alignOfExpr), MATCHER(sizeOfExpr), MATCHER(forEachSwitchCase),
    MATCHER(refersToDeclaration), MATCHER(refersToType), /*MATCHER(findAll),*/
    MATCHER(hasArgumentOfType), MATCHER(hasUnaryOperand),
    MATCHER(hasAnyUsingShadowDecl), MATCHER(hasTargetDecl),
    MATCHER(hasSizeExpr), /*MATCHER2(loc),*/
    MATCHER(hasInitializer)>::result GeneratedAutomata;
GeneratedAutomata Automaton;
}

using namespace CppQuery;

QueryCompleter::QueryCompleter(QObject *parent) : QCompleter(parent) {
  allMatchers <<
#include "matchers.txt"
      ;
}

void QueryCompleter::updateModelFromCtxt(const QString &text,
                                         const QString &prefix) {
  QString parentToken;
  int argPosition = 0;

  int parenLevel = 0;
  for (int i = text.length() - 1; i >= 0; --i) {
    if (text[i] == ')')
      --parenLevel;
    if (text[i] == '(')
      ++parenLevel;
    if (text[i] == ',' && parenLevel == 0)
      ++argPosition;
    if (parenLevel == 1) {
      parentToken = text.left(i);
      int lastComma = parentToken.lastIndexOf(',');
      int lastParen = parentToken.lastIndexOf('(');
      int fromPos = std::max(lastComma, lastParen);
      parentToken = parentToken.mid(fromPos + 1);
      break;
    }
  }

  list.clear();

  if (parentToken.length() != 0) {
    // Update the model
    std::vector<std::string> possibleCompletions =
        Automaton.GetComposables(parentToken.toStdString(), argPosition);

    for (auto &completion : possibleCompletions) {
      QString qcomp = QString::fromStdString(completion);
      if (qcomp.startsWith(prefix) || prefix.length() == 0)
        list.append(qcomp);
    }
  } else {
    for (auto &completion : allMatchers) {
      if (completion.startsWith(prefix))
        list.append(completion);
    }
  }

  model.setStringList(list);
  setModel(&model);
}
