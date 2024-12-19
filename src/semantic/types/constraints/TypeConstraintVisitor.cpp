#include "TypeConstraintVisitor.h"
#include "TipAbsentField.h"
#include "TipAlpha.h"
#include "TipFunction.h"
#include "TipInt.h"
#include "TipRecord.h"
#include "TipRef.h"
#include "TipVar.h"
#include "TipArray.h"
#include "TipBoolean.h"

TypeConstraintVisitor::TypeConstraintVisitor(
    SymbolTable *st, std::shared_ptr<ConstraintHandler> handler)
    : symbolTable(st), constraintHandler(std::move(handler)){};

/*! \fn astToVar
 *  \brief Convert an AST node to a type variable.
 *
 * Utility function that creates type variables and uses declaration nodes
 * as a canonical representative for program variables.  There are two case
 * that need to be checked: if the variable is local to a function or if
 * it is a function value.
 */
std::shared_ptr<TipType> TypeConstraintVisitor::astToVar(ASTNode *n) {
  if (auto ve = dynamic_cast<ASTVariableExpr *>(n)) {
    ASTDeclNode *canonical;
    if ((canonical = symbolTable->getLocal(ve->getName(), scope.top()))) {
      return std::make_shared<TipVar>(canonical);
    } else if ((canonical = symbolTable->getFunction(ve->getName()))) {
      return std::make_shared<TipVar>(canonical);
    }
  } // LCOV_EXCL_LINE

  return std::make_shared<TipVar>(n);
}

bool TypeConstraintVisitor::visit(ASTFunction *element) {
  scope.push(element->getDecl());
  return true;
}

/*! \brief Type constraints for function definition.
 *
 * Type rules for "main(X1, ..., Xn) { ... return E; }":
 *   [[X1]] = [[Xn]] = [[E]] = int
 * To express this we will equate all type variables to int.
 *
 * Type rules for "X(X1, ..., Xn) { ... return E; }":
 *   [[X]] = ([[X1]], ..., [[Xn]]) -> [[E]]
 */
void TypeConstraintVisitor::endVisit(ASTFunction *element) {
  if (element->getName() == "main") {
    std::vector<std::shared_ptr<TipType>> formals;
    for (auto &f : element->getFormals()) {
      formals.push_back(astToVar(f));
      // all formals are int
      constraintHandler->handle(astToVar(f), std::make_shared<TipInt>());
    }

    // Return is the last statement and must be int
    auto ret = dynamic_cast<ASTReturnStmt *>(element->getStmts().back());
    constraintHandler->handle(astToVar(ret->getArg()),
                              std::make_shared<TipInt>());

    constraintHandler->handle(
        astToVar(element->getDecl()),
        std::make_shared<TipFunction>(formals, astToVar(ret->getArg())));
  } else {
    std::vector<std::shared_ptr<TipType>> formals;
    for (auto &f : element->getFormals()) {
      formals.push_back(astToVar(f));
    }

    // Return is the last statement
    auto ret = dynamic_cast<ASTReturnStmt *>(element->getStmts().back());

    constraintHandler->handle(
        astToVar(element->getDecl()),
        std::make_shared<TipFunction>(formals, astToVar(ret->getArg())));
  }
}

/*! \brief Type constraints for numeric literal.
 *
 * Type rules for "I":
 *   [[I]] = int
 */
void TypeConstraintVisitor::endVisit(ASTNumberExpr *element) {
  constraintHandler->handle(astToVar(element), std::make_shared<TipInt>());
}

/*! \brief Type constraints for binary operator.
 *
 * Type rules for "E1 op E2" when "op" is (*, /, %, +, -)
 *   [[E1 op E2]] = int
 * Type rules for "E1 op E2" when "op" is (==, !=, >, >=, <=, <, and, or)
 *   [[E1 op E2]] = boolean
 * When "op" isn't (and, or):
 *   [[E1]] = [[E2]] = int
 * When "op" is (and, or):
 *   [[E1]] = [[E2]] = boolean
 * otherwise
 *   [[E1]] = [[E2]]
 */
void TypeConstraintVisitor::endVisit(ASTBinaryExpr *element) {
  auto op = element->getOp();
  auto intType = std::make_shared<TipInt>();
  auto booleanType = std::make_shared<TipBoolean>();

  std::vector<std::string> intOps = {"*", "/", "+", "-", "%"};
  std::vector<std::string> comparisonOps = {">", ">=", "<=", "<"};

  if (std::find(intOps.begin(), intOps.end(), op) != intOps.end()) {
    constraintHandler->handle(astToVar(element), intType);
  }
  else {
    constraintHandler->handle(astToVar(element), booleanType);
  }

  if (std::find(intOps.begin(), intOps.end(), op) != intOps.end() || std::find(comparisonOps.begin(), comparisonOps.end(), op) != comparisonOps.end()) {
    // operands are integer
    constraintHandler->handle(astToVar(element->getLeft()), intType);
    constraintHandler->handle(astToVar(element->getRight()), intType);
  }
  else if (op == "and" || op == "or") {
    // operands are boolean
    constraintHandler->handle(astToVar(element->getLeft()), booleanType);
    constraintHandler->handle(astToVar(element->getRight()), booleanType);
  } else {
    // operands have the same type
    constraintHandler->handle(astToVar(element->getLeft()),
                              astToVar(element->getRight()));
  }
}

/*! \brief Type constraints for input statement.
 *
 * Type rules for "input":
 *  [[input]] = int
 */
void TypeConstraintVisitor::endVisit(ASTInputExpr *element) {
  constraintHandler->handle(astToVar(element), std::make_shared<TipInt>());
}

/*! \brief Type constraints for function application.
 *
 * Type Rules for "E(E1, ..., En)":
 *  [[E]] = ([[E1]], ..., [[En]]) -> [[E(E1, ..., En)]]
 */
void TypeConstraintVisitor::endVisit(ASTFunAppExpr *element) {
  std::vector<std::shared_ptr<TipType>> actuals;
  for (auto &a : element->getActuals()) {
    actuals.push_back(astToVar(a));
  }
  constraintHandler->handle(
      astToVar(element->getFunction()),
      std::make_shared<TipFunction>(actuals, astToVar(element)));
}

/*! \brief Type constraints for heap allocation.
 *
 * Type Rules for "alloc E":
 *   [[alloc E]] = &[[E]]
 */
void TypeConstraintVisitor::endVisit(ASTAllocExpr *element) {
  constraintHandler->handle(
      astToVar(element),
      std::make_shared<TipRef>(astToVar(element->getInitializer())));
}

/*! \brief Type constraints for address of.
 *
 * Type Rules for "&X":
 *   [[&X]] = &[[X]]
 */
void TypeConstraintVisitor::endVisit(ASTRefExpr *element) {
  constraintHandler->handle(
      astToVar(element), std::make_shared<TipRef>(astToVar(element->getVar())));
}

/*! \brief Type constraints for pointer dereference.
 *
 * Type Rules for "*E":
 *   [[E]] = &[[*E]]
 */
void TypeConstraintVisitor::endVisit(ASTDeRefExpr *element) {
  constraintHandler->handle(astToVar(element->getPtr()),
                            std::make_shared<TipRef>(astToVar(element)));
}

/*! \brief Type constraints for null literal.
 *
 * Type Rules for "null":
 *   [[null]] = & \alpha
 */
void TypeConstraintVisitor::endVisit(ASTNullExpr *element) {
  constraintHandler->handle(
      astToVar(element),
      std::make_shared<TipRef>(std::make_shared<TipAlpha>(element)));
}

/*! \brief Type rules for assignments.
 *
 * Type rules for "E1 = E":
 *   [[E1]] = [[E2]]
 *
 * Type rules for "*E1 = E2":
 *   [[E1]] = &[[E2]]
 *
 * Note that these are slightly more general than the rules in the SPA book.
 * The first allows for record expressions on the left hand side and the second
 * allows for more complex assignments, e.g., "**p = &x"
 */
void TypeConstraintVisitor::endVisit(ASTAssignStmt *element) {
  // If this is an assignment through a pointer, use the second rule above
  if (auto lptr = dynamic_cast<ASTDeRefExpr *>(element->getLHS())) {
    constraintHandler->handle(
        astToVar(lptr->getPtr()),
        std::make_shared<TipRef>(astToVar(element->getRHS())));
  } else {
    constraintHandler->handle(astToVar(element->getLHS()),
                              astToVar(element->getRHS()));
  }
}

/*! \brief Type constraints for while loop.
 *
 * Type rules for "while (E) S":
 *   [[E]] = boolean
 */
void TypeConstraintVisitor::endVisit(ASTWhileStmt *element) {
  constraintHandler->handle(astToVar(element->getCondition()),
                            std::make_shared<TipBoolean>());
}

/*! \brief Type constraints for if statement.
 *
 * Type rules for "if (E) S1 else S2":
 *   [[E]] = boolean
 */
void TypeConstraintVisitor::endVisit(ASTIfStmt *element) {
  constraintHandler->handle(astToVar(element->getCondition()),
                            std::make_shared<TipBoolean>());
}

/*! \brief Type constraints for output statement.
 *
 * Type rules for "output E":
 *   [[E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTOutputStmt *element) {
  constraintHandler->handle(astToVar(element->getArg()),
                            std::make_shared<TipInt>());
}

/*! \brief Type constraints for record expression.
 *
 * Type rule for "{ X1:E1, ..., Xn:En }":
 *   [[{ X1:E1, ..., Xn:En }]] = { f1:v1, ..., fn:vn }
 * where fi is the ith field in the program's global record
 * and vi = [[Ei]] if fi = Xi and \alpha otherwise
 */
void TypeConstraintVisitor::endVisit(ASTRecordExpr *element) {
  auto allFields = symbolTable->getFields();
  std::vector<std::shared_ptr<TipType>> fieldTypes;
  for (auto &f : allFields) {
    bool matched = false;
    for (auto &fe : element->getFields()) {
      if (f == fe->getField()) {
        fieldTypes.push_back(astToVar(fe->getInitializer()));
        matched = true;
        break;
      }
    }
    if (matched)
      continue;

    fieldTypes.push_back(std::make_shared<TipAbsentField>());
  }
  constraintHandler->handle(astToVar(element),
                            std::make_shared<TipRecord>(fieldTypes, allFields));
}

/*! \brief Type constraints for field access.
 *
 * Type rule for "E.X":
 *   [[E]] = { f1:v1, ..., fn:vn }
 * where fi is the ith field in the program's global record
 * and vi = [[E.X]] if fi = X and \alpha otherwise
 */
void TypeConstraintVisitor::endVisit(ASTAccessExpr *element) {
  auto allFields = symbolTable->getFields();
  std::vector<std::shared_ptr<TipType>> fieldTypes;
  for (auto &f : allFields) {
    if (f == element->getField()) {
      fieldTypes.push_back(astToVar(element));
    } else {
      fieldTypes.push_back(std::make_shared<TipAlpha>(element, f));
    }
  }
  constraintHandler->handle(astToVar(element->getRecord()),
                            std::make_shared<TipRecord>(fieldTypes, allFields));
}

/*! \brief Type constraints for error statement.
 *
 * Type rules for "error E":
 *   [[E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTErrorStmt *element) {
  constraintHandler->handle(astToVar(element->getArg()),
                            std::make_shared<TipInt>());
}

/*! \brief Type constraints for a default array.
 *
 * Type rules for "[E1, E2, ..., En]":
 *   [[ [E1, E2, ..., En] ]] = array[ [[Ei]] ] for all valid i values (all elements have the same type)
 * Furthermore, the rule can be shown as:
 *   [[ [E1, E2, ..., En] ]] = array[ [[\alpha]] ], meaning any valid type can be held in an array
 */
void TypeConstraintVisitor::endVisit(ASTArrayDefaultExpr *element) {
  constraintHandler->handle(astToVar(element), std::make_shared<TipArray>(std::make_shared<TipAlpha>(element)));
  for (ASTExpr *currentElement: element->getFields()) {
    constraintHandler->handle(astToVar(element), std::make_shared<TipArray>(astToVar(currentElement)));
  }
}

/*! \brief Type constraints for a fixed array.
 *
 * Type rules for "[E1 of E2]":
 *   [[E1]] = int
 *   [[ [E1 of E2] ]] = array([[E2]])
 */
void TypeConstraintVisitor::endVisit(ASTArrayFixedExpr *element) {
  constraintHandler->handle(astToVar(element->getNumber()), std::make_shared<TipInt>());
  constraintHandler->handle(astToVar(element), std::make_shared<TipArray>(astToVar(element->getInstance())));
}

/*! \brief Type constraints for increment statement.
 *
 * Type rules for "E++":
 *   [[E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTIncrementStmt *element) {
  constraintHandler->handle(astToVar(element->getBase()),
                            std::make_shared<TipInt>());
}

/*! \brief Type constraints for decrement statement.
 *
 * Type rules for "E--":
 *   [[E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTDecrementStmt *element) {
  constraintHandler->handle(astToVar(element->getBase()),
                            std::make_shared<TipInt>());
}

/*! \brief Type constraints for ternary expression.
 *
 * Type rules for "E1 ? E2 : E3":
 *   [[E1]] = boolean
 *   [[E2]] = [[E3]] = [[E1 ? E2 : E3]]
 */
void TypeConstraintVisitor::endVisit(ASTTernaryExpr *element) {
  constraintHandler->handle(astToVar(element->getCondition()), std::make_shared<TipBoolean>());
  constraintHandler->handle(astToVar(element), astToVar(element->getThen()));
  constraintHandler->handle(astToVar(element), astToVar(element->getElse()));
}

/*! \brief Type constraints for boolean value.
 *
 * Type rules for "B":
 *   [[B]] = boolean
 */
void TypeConstraintVisitor::endVisit(ASTBooleanExpr *element) {
  constraintHandler->handle(astToVar(element), std::make_shared<TipBoolean>());
}

/*! \brief Type constraints for negation operation.
 *
 * Type rules for "-E":
 *   [[-E]] = [[E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTNegExpr *element) {
  constraintHandler->handle(astToVar(element), std::make_shared<TipInt>());
  constraintHandler->handle(astToVar(element->getExpr()), std::make_shared<TipInt>());
}

/*! \brief Type constraints for not operation.
 *
 * Type rules for "not E":
 *   [[not E]] = [[E]] = boolean
 */
void TypeConstraintVisitor::endVisit(ASTNotExpr *element) {
  constraintHandler->handle(astToVar(element), std::make_shared<TipBoolean>());
  constraintHandler->handle(astToVar(element->getBase()), std::make_shared<TipBoolean>());
}

/*! \brief Type constraints for iterator-style for loop.
 *
 * Type rules for "for (E1 : E2) S":
 *   [[E2]] = array([[E1]])
 */
void TypeConstraintVisitor::endVisit(ASTForIteratorStmt *element) {
    constraintHandler->handle(astToVar(element->getIterable()), std::make_shared<TipArray>(astToVar(element->getElement())));
}

/*! \brief Type constraints for range-style for loop.
 *
 * Type rules for "for (E1 : E2 .. E3 by E4) S":
 *   [[E1]] = [[E2]] = [[E3]] = [[E4]] = int
 */
void TypeConstraintVisitor::endVisit(ASTForRangeStmt *element) {
    constraintHandler->handle(astToVar(element->getElement()), std::make_shared<TipInt>());
    constraintHandler->handle(astToVar(element->getLower()), std::make_shared<TipInt>());
    constraintHandler->handle(astToVar(element->getUpper()), std::make_shared<TipInt>());
    constraintHandler->handle(astToVar(element->getStep()), std::make_shared<TipInt>());
}

/*! \brief Type constraints for array length expression.
 *
 * Type rules for "#E":
 *   [[E]] = array(\alpha)
 *   [[#E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTArrayLenExpr *element) {
    constraintHandler->handle(astToVar(element->getArray()), std::make_shared<TipArray>(std::make_shared<TipAlpha>(element->getArray())));
    constraintHandler->handle(astToVar(element), std::make_shared<TipInt>());
}

/*! \brief Type constraints for array reference expression.
 *
 * Type rules for "E1[E2]":
 *  [[E1]] = array([[E1[E2]]])
 *  [[E2]] = int
 *
 */
void TypeConstraintVisitor::endVisit(ASTArrayRefExpr *element) {
    constraintHandler->handle(astToVar(element->getArray()), std::make_shared<TipArray>(astToVar(element)));
    constraintHandler->handle(astToVar(element->getIndex()), std::make_shared<TipInt>());
}
