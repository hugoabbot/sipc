#include "ASTHelper.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

#include <sstream>
#include "ASTVisitor.h"

TEST_CASE("SIPASTNodeTest: ternary expression", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
    foo(c) {
      var x, y, z;
      x = x == 0 ? y : z + (y - x);
      return x;
    }
  )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTTernaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getCondition();
  REQUIRE(o1.str() == "(x==0)");

  std::stringstream o2;
  o2 << *expr->getThen();
  REQUIRE(o2.str() == "y");

  std::stringstream o3;
  o3 << *expr->getElse();
  REQUIRE(o3.str() == "(z+(y-x))");

  REQUIRE(expr->getChildren().size() == 3);
}

TEST_CASE("SIPASTNodeTest: increment statement", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
    foo(c) {
      var x;
      x++;
      return x;
    }
  )";

  auto ast = ASTHelper::build_ast(stream);
  auto stmt = ASTHelper::find_node<ASTIncrementStmt>(ast);

  std::stringstream o1;
  o1 << *stmt->getBase();
  REQUIRE(o1.str() == "x");

  REQUIRE(stmt->getChildren().size() == 1);
}

TEST_CASE("SIPASTNodeTest: decrement statement", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
    foo(c) {
      var x;
      x--;
      return x;
    }
  )";

  auto ast = ASTHelper::build_ast(stream);
  auto stmt = ASTHelper::find_node<ASTDecrementStmt>(ast);

  std::stringstream o1;
  o1 << *stmt->getBase();
  REQUIRE(o1.str() == "x");

  REQUIRE(stmt->getChildren().size() == 1);
}

TEST_CASE("SIPASTNodeTest: boolean type (true)", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
    foo(c) {
      var x;
      x = true;
      return x;
    }
  )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBooleanExpr>(ast);
  REQUIRE(expr->getValue() == 1);

  auto value = std::make_unique<ASTBooleanExpr>(1);

  REQUIRE(value->getValue() == true);
}

TEST_CASE("SIPASTNodeTest: boolean type (false)", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
    foo(c) {
      var x;
      x = false;
      return x;
    }
  )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBooleanExpr>(ast);
  REQUIRE(expr->getValue() == 0);

  auto value = std::make_unique<ASTBooleanExpr>(0);

  REQUIRE(value->getValue() == false);
}

TEST_CASE("SIPASTNodeTest: default array", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
    foo(c) {
      var x;
      x = [1, y, 89];
      return x;
    }
  )";

  auto ast = ASTHelper::build_ast(stream);
  auto array = ASTHelper::find_node<ASTArrayDefaultExpr>(ast);

  std::stringstream o1;
  o1 << *array->getFields()[0];
  REQUIRE(o1.str() == "1");

  std::stringstream o2;
  o2 << *array->getFields()[1];
  REQUIRE(o2.str() == "y");

  std::stringstream o3;
  o3 << *array->getFields()[2];
  REQUIRE(o3.str() == "89");

  REQUIRE(array->getChildren().size() == 3);
}

TEST_CASE("SIPASTNodeTest: default array empty", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
    foo() {
      var x;
      x = [];
      return x;
    }
  )";

  auto ast = ASTHelper::build_ast(stream);
  auto array = ASTHelper::find_node<ASTArrayDefaultExpr>(ast);

  REQUIRE(array->getChildren().size() == 0);
}

TEST_CASE("SIPASTNodeTest: fixed array", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
    foo(c) {
      var x, y;
      x = 9;
      y = [x of 3];
      return x;
    }
  )";

  auto ast = ASTHelper::build_ast(stream);
  auto array = ASTHelper::find_node<ASTArrayFixedExpr>(ast);
  
  REQUIRE(array->getChildren().size() == 2);

  std::stringstream o1;
  o1 << *array->getNumber();
  REQUIRE(o1.str() == "x");
  o1.str("");
  o1 << *array->getInstance();
  REQUIRE(o1.str() == "3");
}

TEST_CASE("SIPASTNodeTest: unary not", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
    foo(c) {
      var x, y;
      x = not y;
      return x;
    }
  )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTNotExpr>(ast);

  REQUIRE(expr->getChildren().size() == 1);

  std::stringstream o1;
  o1 << *expr->getBase();
  REQUIRE(o1.str() == "y");
}

TEST_CASE("ASTForIteratorStmt: Iterator-style for loop", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(a) {
         var s, x;
         s = 0;
         for (x : a) {
            s = s + x;
         }
         return s;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto stmt = ASTHelper::find_node<ASTForIteratorStmt>(ast);

  std::stringstream o1;
  o1 << *stmt->getElement();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *stmt->getIterable();
  REQUIRE(o2.str() == "a");

  std::stringstream o3;
  o3 << *stmt->getBody();
  REQUIRE(o3.str() == "{ s = (s+x); }");

  std::stringstream p;
  p << *stmt;
  REQUIRE(p.str() == "for (x : a) { s = (s+x); }");

  auto children = stmt->getChildren();
  REQUIRE(children.size() == 3);
}

TEST_CASE("ASTForRangeStmt: Range-style for loop without 'by'", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(y) {
         var s, x;
         s = 0;
         for (x : 0 .. y) {
            s = s + x;
         }
         return s;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto stmt = ASTHelper::find_node<ASTForRangeStmt>(ast);

  std::stringstream o1;
  o1 << *stmt->getElement();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *stmt->getLower();
  REQUIRE(o2.str() == "0");

  std::stringstream o3;
  o3 << *stmt->getUpper();
  REQUIRE(o3.str() == "y");

  std::stringstream o4;
  o4 << *stmt->getStep();
  REQUIRE(o4.str() == "1");

  std::stringstream o5;
  o5 << *stmt->getBody();
  REQUIRE(o5.str() == "{ s = (s+x); }");

  std::stringstream p;
  p << *stmt;
  REQUIRE(p.str() == "for (x : 0 .. y by 1) { s = (s+x); }");

  auto children = stmt->getChildren();
  REQUIRE(children.size() == 5);
}

TEST_CASE("ASTForRangeStmt: Range-style for loop with 'by'", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(y) {
         var s, x;
         s = 0;
         for (x : 0 .. y by 2) {
            s = s + x;
         }
         return s;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto stmt = ASTHelper::find_node<ASTForRangeStmt>(ast);

  std::stringstream o1;
  o1 << *stmt->getElement();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *stmt->getLower();
  REQUIRE(o2.str() == "0");

  std::stringstream o3;
  o3 << *stmt->getUpper();
  REQUIRE(o3.str() == "y");

  std::stringstream o4;
  o4 << *stmt->getStep();
  REQUIRE(o4.str() == "2");

  std::stringstream o5;
  o5 << *stmt->getBody();
  REQUIRE(o5.str() == "{ s = (s+x); }");

  std::stringstream p;
  p << *stmt;
  REQUIRE(p.str() == "for (x : 0 .. y by 2) { s = (s+x); }");

  auto children = stmt->getChildren();
  REQUIRE(children.size() == 5);
}

TEST_CASE("ASTNegExpr: negated expression", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y;
         y = - (x + 3);
         return y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTNegExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getExpr();
  REQUIRE(o1.str() == "(x+3)");

  std::stringstream p;
  p << *expr;
  REQUIRE(p.str() == "-((x+3))");

  auto children = expr->getChildren();
  REQUIRE(children.size() == 1);
}

TEST_CASE("ASTArrayLenExpr: array length expression", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y;
         y = #[x of 1];
         return y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTArrayLenExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getArray();
  REQUIRE(o1.str() == "[x of 1]");

  std::stringstream p;
  p << *expr;
  REQUIRE(p.str() == "#[x of 1]");

  auto children = expr->getChildren();
  REQUIRE(children.size() == 1);
}

TEST_CASE("ASTArrayRefExpr: array reference expression", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(x, a) {
         return a[x];
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTArrayRefExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getArray();
  REQUIRE(o1.str() == "a");

  std::stringstream o2;
  o2 << *expr->getIndex();
  REQUIRE(o2.str() == "x");

  std::stringstream p;
  p << *expr;
  REQUIRE(p.str() == "a[x]");

  auto children = expr->getChildren();
  REQUIRE(children.size() == 2);
}

TEST_CASE("ASTBinaryExpr: binary expression with mod", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(x) {
         return 42 % x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "42");

  std::stringstream o2;
  o2 << expr->getOp();
  REQUIRE(o2.str() == "%");

  std::stringstream o3;
  o3 << *expr->getRight();
  REQUIRE(o3.str() == "x");
}

TEST_CASE("ASTBinaryExpr: binary expression with and", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(x) {
         return x and true;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << expr->getOp();
  REQUIRE(o2.str() == "and");

  std::stringstream o3;
  o3 << *expr->getRight();
  REQUIRE(o3.str() == "true");
}

TEST_CASE("ASTBinaryExpr: binary expression with or", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(x) {
         return false or x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "false");

  std::stringstream o2;
  o2 << expr->getOp();
  REQUIRE(o2.str() == "or");

  std::stringstream o3;
  o3 << *expr->getRight();
  REQUIRE(o3.str() == "x");
}

TEST_CASE("ASTBinaryExpr: binary expression with LT", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(x) {
         return x < 42;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << expr->getOp();
  REQUIRE(o2.str() == "<");

  std::stringstream o3;
  o3 << *expr->getRight();
  REQUIRE(o3.str() == "42");
}

TEST_CASE("ASTBinaryExpr: binary expression with LTE", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(x) {
         return x <= 42;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << expr->getOp();
  REQUIRE(o2.str() == "<=");

  std::stringstream o3;
  o3 << *expr->getRight();
  REQUIRE(o3.str() == "42");
}

TEST_CASE("ASTBinaryExpr: binary expression with GTE", "[SIPASTNode]") {
  std::stringstream stream;
  stream << R"(
      foo(x) {
         return x >= 42;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << expr->getOp();
  REQUIRE(o2.str() == ">=");

  std::stringstream o3;
  o3 << *expr->getRight();
  REQUIRE(o3.str() == "42");
}
