#include "ASTHelper.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

TEST_CASE("SIPASTPrinterTest: ternary output test", "[SIPASTNodePrint]") {
  std::stringstream stream;
  auto x = std::make_shared<ASTVariableExpr>("x");
  auto y = std::make_shared<ASTVariableExpr>("y");
  auto z = std::make_shared<ASTVariableExpr>("z");

  auto ternary = std::make_unique<ASTTernaryExpr>(std::move(x), std::move(y), std::move(z));
  stream << *ternary;

  REQUIRE(stream.str() == "x ? y : z");
}

TEST_CASE("SIPASTPrinterTest: increment output test", "[SIPASTNodePrint]") {
  std::stringstream stream;
  auto x = std::make_shared<ASTVariableExpr>("x");
  auto expr = std::make_unique<ASTIncrementStmt>(std::move(x));
  stream << *expr;

  REQUIRE(stream.str() == "x++");
}

TEST_CASE("SIPASTPrinterTest: decrement output test", "[SIPASTNodePrint]") {
  std::stringstream stream;
  auto x = std::make_shared<ASTVariableExpr>("x");
  auto expr = std::make_unique<ASTDecrementStmt>(std::move(x));
  stream << *expr;

  REQUIRE(stream.str() == "x--");
}

TEST_CASE("SIPASTPrinterTest: boolean output test (both true and false)", "[SIPASTNodePrint]") {
  std::stringstream stream;
  auto value = std::make_unique<ASTBooleanExpr>(1);
  stream << *value;

  REQUIRE(stream.str() == "true");

  value = std::make_unique<ASTBooleanExpr>(0);
  stream.str("");
  stream << *value;

  REQUIRE(stream.str() == "false");
}

TEST_CASE("SIPASTPrinterTest: default array output test", "[SIPASTNodePrint]") {
  std::stringstream stream;
  auto int1 = std::make_shared<ASTNumberExpr>(0);
  auto y = std::make_shared<ASTVariableExpr>("y");
  auto int2 = std::make_shared<ASTNumberExpr>(86);
  std::vector<std::shared_ptr<ASTExpr>> list;
  list.push_back(int1);
  list.push_back(y);
  list.push_back(int2);

  auto expr = std::make_unique<ASTArrayDefaultExpr>(std::move(list));
  stream.str("");
  stream << *expr;

  REQUIRE(stream.str() == "[0, y, 86]");
}

TEST_CASE("SIPASTPrinterTest: fixed array output test", "[SIPASTNodePrint]") {
  std::stringstream stream;
  auto number = std::make_shared<ASTVariableExpr>("x");
  auto instance = std::make_shared<ASTNumberExpr>(3);

  auto expr = std::make_unique<ASTArrayFixedExpr>(std::move(number), std::move(instance));
  stream.str("");
  stream << *expr;

  REQUIRE(stream.str() == "[x of 3]");
}

TEST_CASE("SIPASTPrinterTest: unary not output test", "[SIPASTNodePrint]") {
  std::stringstream stream;
  auto x = std::make_shared<ASTVariableExpr>("x");

  auto expr2 = std::make_unique<ASTNotExpr>(std::move(x));
  stream.str("");
  stream << *expr2;

  REQUIRE(stream.str() == "not (x)");
}
