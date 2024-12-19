#include "PrettyPrinter.h"

#include <iostream>
#include <sstream>

void PrettyPrinter::print(ASTProgram *p, std::ostream &os, char c, int n) {
  PrettyPrinter visitor(os, c, n);
  p->accept(&visitor);
} // LCOV_EXCL_LINE

/**
 * join last sz items from the visitedResults with delimiter delim. Then, remove
 * these items from the stack
 *
 * the delimiter before the last skip items are not appended (e.g. to not add
 * trailing commas)
 *
 * returns the joined string
 */
std::string joinWithDelim(std::vector<std::string> &visitResults,
                          std::string delim, int sz, int skip) {
  std::string out;
  int i;
  for (i = visitResults.size() - sz; i < visitResults.size() - skip; i++) {
    out += visitResults[i];
    out += delim;
  }
  // skip the delimiter for the last several tokens
  for (; i < visitResults.size(); i++)
    out += visitResults[i];
  visitResults.erase(visitResults.begin() + visitResults.size() - sz,
                     visitResults.end());
  return out;
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTProgram *element) {
  os << joinWithDelim(visitResults, "\n", element->getFunctions().size(), 1);
  os.flush();
} // LCOV_EXCL_LINE

/*
 * General approach taken by visit methods.
 * - visit() is used to increase indentation (decrease should happen in
 * endVisit).
 * - endVisit() should expect a string for all of its AST nodes in reverse order
 * in visitResults. Communicate the single string for the visited node by
 * pushing to the back of visitedResults.
 */

/*
 * Before visiting function, record string for signature and setup indentation
 * for body. This visit method pushes a string result, that the endVisit method
 * should extend.
 */
bool PrettyPrinter::visit(ASTFunction *element) {
  indentLevel++;
  return true;
} // LCOV_EXCL_LINE

/*
 * After visiting function, collect the string representations for the:
 *   statements, declarations, formals, and then function name
 * they are on the visit stack in that order.
 */
void PrettyPrinter::endVisit(ASTFunction *element) {
  auto bodyString =
      joinWithDelim(visitResults, "\n", element->getStmts().size(), 0);
  auto declString =
      joinWithDelim(visitResults, "\n", element->getDeclarations().size(), 0);
  auto formalsString =
      joinWithDelim(visitResults, ", ", element->getFormals().size(), 1);
  auto polyString = element->isPoly() ? "poly" : "";

  // function name is last element on stack, we modify it in place
  visitResults.back() += "(" + formalsString + ") " + polyString + "\n{\n" +
                         declString + bodyString + "}\n";
  indentLevel--;
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTNumberExpr *element) {
  visitResults.push_back(std::to_string(element->getValue()));
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTVariableExpr *element) {
  visitResults.push_back(element->getName());
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTBinaryExpr *element) {
  std::string rightString = visitResults.back();
  visitResults.pop_back();
  std::string leftString = visitResults.back();
  visitResults.pop_back();

  visitResults.push_back("(" + leftString + " " + element->getOp() + " " +
                         rightString + ")");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTInputExpr *element) {
  visitResults.push_back("input");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTFunAppExpr *element) {
  auto actualsString =
      joinWithDelim(visitResults, ", ", element->getActuals().size(), 1);
  visitResults.back() += "(" + actualsString + ")";
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTAllocExpr *element) {
  std::string init = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back("alloc " + init);
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTRefExpr *element) {
  std::string var = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back("&" + var);
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTDeRefExpr *element) {
  std::string base = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back("*" + base);
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTNullExpr *element) {
  visitResults.push_back("null");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTFieldExpr *element) {
  std::string init = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(element->getField() + ":" + init);
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTRecordExpr *element) {
  visitResults.push_back(
      "{" + joinWithDelim(visitResults, ", ", element->getFields().size(), 1) +
      "}");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTAccessExpr *element) {
  std::string accessString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(accessString + '.' + element->getField());
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTDeclNode *element) {
  visitResults.push_back(element->getName());
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTDeclStmt *element) {
  visitResults.push_back(
      indent() + "var " +
      joinWithDelim(visitResults, ", ", element->getVars().size(), 1) + ";");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTAssignStmt *element) {
  std::string rhsString = visitResults.back();
  visitResults.pop_back();
  std::string lhsString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + lhsString + " = " + rhsString + ";");
} // LCOV_EXCL_LINE

bool PrettyPrinter::visit(ASTBlockStmt *element) {
  indentLevel++;
  return true;
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTBlockStmt *element) {
  indentLevel--;
  visitResults.push_back(
      indent() + "{\n" +
      joinWithDelim(visitResults, "\n", element->getStmts().size(), 0) +
      indent() + "}");
} // LCOV_EXCL_LINE

/*
 * For a while the body should be indented, but not the condition.
 * Since conditions are expressions and their visit methods never indent
 * incrementing here works.
 */
bool PrettyPrinter::visit(ASTWhileStmt *element) {
  indentLevel++;
  return true;
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTWhileStmt *element) {
  std::string bodyString = visitResults.back();
  visitResults.pop_back();
  std::string condString = visitResults.back();
  visitResults.pop_back();

  indentLevel--;

  std::string whileString =
      indent() + "while (" + condString + ") \n" + bodyString;
  visitResults.push_back(whileString);
} // LCOV_EXCL_LINE

bool PrettyPrinter::visit(ASTIfStmt *element) {
  indentLevel++;
  return true;
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTIfStmt *element) {
  std::string elseString;
  if (element->getElse() != nullptr) {
    elseString = visitResults.back();
    visitResults.pop_back();
  }

  std::string thenString = visitResults.back();
  visitResults.pop_back();

  std::string condString = visitResults.back();
  visitResults.pop_back();

  indentLevel--;

  std::string ifString = indent() + "if (" + condString + ") \n" + thenString;

  if (element->getElse() != nullptr) {
    ifString += "\n" + indent() + "else\n" + elseString;
  }

  visitResults.push_back(ifString);
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTOutputStmt *element) {
  std::string argString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + "output " + argString + ";");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTErrorStmt *element) {
  std::string argString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + "error " + argString + ";");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTReturnStmt *element) {
  std::string argString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + "return " + argString + ";");
} // LCOV_EXCL_LINE

std::string PrettyPrinter::indent() const {
  return std::string(indentLevel * indentSize, indentChar);
} // LCOV_EXCL_LINE

// Begin SIP extension
void PrettyPrinter::endVisit(ASTTernaryExpr *element) {
  std::string ELSE = visitResults.back();
  visitResults.pop_back();
  std::string THEN = visitResults.back();
  visitResults.pop_back();
  std::string COND = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(COND + " ? " + THEN + " : " + ELSE);
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTIncrementStmt *element) {
  std::string base = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + base + "++;");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTDecrementStmt *element) {
  std::string base = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + base + "--;");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTBooleanExpr *element) {
  visitResults.push_back(element->getValue() ? "true" : "false");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTArrayDefaultExpr *element) {
  std::string elements = joinWithDelim(visitResults, ", ", element->getFields().size(), 1);
  visitResults.push_back("[" + elements + "]");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTArrayFixedExpr *element) {
  std::string instance = visitResults.back();
  visitResults.pop_back();
  std::string number = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back("[" + number + " of " + instance + "]");
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTNotExpr *element) {
  std::string base = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back("not (" + base + ")");
} // LCOV_EXCL_LINE

bool PrettyPrinter::visit(ASTForIteratorStmt *element) {
  indentLevel++;
  return true;
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTForIteratorStmt *element) {
  std::string bodyString = visitResults.back();
  visitResults.pop_back();
  std::string iterableString = visitResults.back();
  visitResults.pop_back();
  std::string elementString = visitResults.back();
  visitResults.pop_back();

  indentLevel--;

  std::string forIteratorString =
      indent() + "for (" + elementString + " : " + iterableString + ") \n" + bodyString;
  visitResults.push_back(forIteratorString);
} // LCOV_EXCL_LINE

bool PrettyPrinter::visit(ASTForRangeStmt *element) {
  indentLevel++;
  return true;
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTForRangeStmt *element) {
  std::string bodyString = visitResults.back();
  visitResults.pop_back();
  std::string stepString = visitResults.back();
  visitResults.pop_back();
  std::string upperString = visitResults.back();
  visitResults.pop_back();
  std::string lowerString = visitResults.back();
  visitResults.pop_back();
  std::string elementString = visitResults.back();
  visitResults.pop_back();

  indentLevel--;

  std::string forRangeString =
      indent() + "for (" + elementString + " : " + lowerString + " .. " + upperString + " by " + stepString + ") \n" + bodyString;
  visitResults.push_back(forRangeString);
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTNegExpr *element) {
  std::string exprString = visitResults.back();
  visitResults.pop_back();
  std::string negExprString = "-(" + exprString + ")";
  visitResults.push_back(negExprString);
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTArrayLenExpr *element) {
  std::string arrayString = visitResults.back();
  visitResults.pop_back();
  std::string arrayLenString = "#" + arrayString;
  visitResults.push_back(arrayLenString);
} // LCOV_EXCL_LINE

void PrettyPrinter::endVisit(ASTArrayRefExpr *element) {
  std::string indexString = visitResults.back();
  visitResults.pop_back();
  std::string arrayString = visitResults.back();
  visitResults.pop_back();

  std::string arrayRefString = arrayString + "[" + indexString + "]";
  visitResults.push_back(arrayRefString);
} // LCOV_EXCL_LINE
