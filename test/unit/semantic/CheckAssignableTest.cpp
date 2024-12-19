#include "CheckAssignable.h"
#include "ASTHelper.h"
#include "ExceptionContainsWhat.h"
#include "SemanticError.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

TEST_CASE("Check Assignable: variable lhs", "[Symbol]") {
  std::stringstream stream;
  stream << R"(varlhs() { var x; x = 1; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

TEST_CASE("Check Assignable: pointer lhs", "[Symbol]") {
  std::stringstream stream;
  stream << R"(ptrlhs() { var x; *x = 1; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

TEST_CASE("Check Assignable: field lhs", "[Symbol]") {
  std::stringstream stream;
  stream << R"(fieldlhs() { var x; x = {f:0, g:1}; x.f = 1; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

TEST_CASE("Check Assignable: complex field lhs", "[Symbol]") {
  std::stringstream stream;
  stream << R"(recordlhs() { var x; {f:0, g:1}.f = x; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("{f:0,g:1} is an expression, and not a "
                                      "variable corresponding to a record"));
}

TEST_CASE("Check Assignable: complex pointer lhs", "[Symbol]") {
  std::stringstream stream;
  stream
      << R"(foo(x) { return &x; } ptrlhs() { var x; *foo(&x) = 1; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

TEST_CASE("Check Assignable: address of var", "[Symbol]") {
  std::stringstream stream;
  stream << R"(recordlhs() { var x, y; x = &y; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

TEST_CASE("Check Assignable: address of field", "[Symbol]") {
  std::stringstream stream;
  stream
      << R"(recordlhs() { var x, y; y = {f:0, g:1}; x = &(y.g); return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

// Begin SIP extension

TEST_CASE("Check Assignable: increment statement", "[Symbol]") {
  std::stringstream stream;
  stream << R"(ptrlhs() { var x; x++; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

TEST_CASE("Check Assignable: decrement statement", "[Symbol]") {
  std::stringstream stream;
  stream << R"(ptrlhs() { var x; x--; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

TEST_CASE("Check Assignable: iterator-style for loop element", "[Symbol]") {
  std::stringstream stream;
  stream << R"(for_it(a) { var s, x; s = 0; for(x : a) {s = s + x;} return s; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

TEST_CASE("Check Assignable: range-style for loop element", "[Symbol]") {
  std::stringstream stream;
  stream << R"(for_range() { var s, x; s = 0; for(x : 0 .. 10 by 2) {s = s + x;} return s; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_NOTHROW(CheckAssignable::check(ast.get()));
}

// End SIP extension

/************** the following are expected to fail the check ************/

TEST_CASE("Check Assignable: constant lhs", "[Symbol]") {
  std::stringstream stream;
  stream << R"(constlhs() { var x; 7 = x; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("7 not an l-value"));
}

TEST_CASE("Check Assignable: binary lhs", "[Symbol]") {
  std::stringstream stream;
  stream << R"(binlhs() { var x; x+1 = x; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("(x+1) not an l-value"));
}

TEST_CASE("Check Assignable: function lhs", "[Symbol]") {
  std::stringstream stream;
  stream << R"(foo() { return 0; } funlhs() { var x; foo() = x; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("foo() not an l-value"));
}

TEST_CASE("Check Assignable: alloc lhs", "[Symbol]") {
  std::stringstream stream;
  stream << R"(alloclhs() { var x; alloc 1 = x; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("alloc 1 not an l-value"));
}

TEST_CASE("Check Assignable: record lhs", "[Symbol]") {
  std::stringstream stream;
  stream << R"(recordlhs() { var x; {f:0, g:1} = x; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("{f:0,g:1} not an l-value"));
}

TEST_CASE("Check Assignable: address of pointer", "[Symbol]") {
  std::stringstream stream;
  stream << R"(recordlhs(p) { var x; x = &(*p); return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("(*p) not an l-value"));
}

TEST_CASE("Check Assignable: address of expr", "[Symbol]") {
  std::stringstream stream;
  stream << R"(recordlhs(p) { var x, y; x = &(y*y); return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("(y*y) not an l-value"));
}

// Begin SIP extension

TEST_CASE("Check Assignable: increment on constant", "[Symbol]") {
  std::stringstream stream;
  stream << R"(constinc() { 5++; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("5 not an l-value"));
}

TEST_CASE("Check Assignable: decrement on constant", "[Symbol]") {
  std::stringstream stream;
  stream << R"(constdec() { 5--; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("5 not an l-value"));
}

TEST_CASE("Check Assignable: increment on binary expression", "[Symbol]") {
  std::stringstream stream;
  stream << R"(bininc() { var x; (x + 1)++; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("(x+1) not an l-value"));
}

TEST_CASE("Check Assignable: decrement on binary expression", "[Symbol]") {
  std::stringstream stream;
  stream << R"(bindec() { var x; (x + 1)--; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("(x+1) not an l-value"));
}

TEST_CASE("Check Assignable: increment on function call", "[Symbol]") {
  std::stringstream stream;
  stream << R"(funinc() { var x; foo()++; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("foo() not an l-value"));
}

TEST_CASE("Check Assignable: decrement on function call", "[Symbol]") {
  std::stringstream stream;
  stream << R"(fundec() { var x; foo()--; return 0; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("foo() not an l-value"));
}

TEST_CASE("Check Assignable: constant as iterator-style for loop element", "[Symbol]") {
  std::stringstream stream;
  stream << R"(for_it(a) { var s, x; s = 0; for(42 : a) {s = s + x;} return s; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("42 not an l-value"));
}

TEST_CASE("Check Assignable: function call as iterator-style for loop element", "[Symbol]") {
  std::stringstream stream;
  stream << R"(for_it(a) { var s, x; s = 0; for(foo() : a) {s = s + x;} return s; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("foo() not an l-value"));
}

TEST_CASE("Check Assignable: binary expression as iterator-style for loop element", "[Symbol]") {
  std::stringstream stream;
  stream << R"(for_it(a) { var s, x; s = 0; for(x + 1 : a) {s = s + x;} return s; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("(x+1) not an l-value"));
}

TEST_CASE("Check Assignable: constant as range-style for loop element", "[Symbol]") {
  std::stringstream stream;
  stream << R"(for_range() { var s, x; s = 0; for(42 : 0 .. 10 by 2) {s = s + x;} return s; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("42 not an l-value"));
}

TEST_CASE("Check Assignable: function call as range-style for loop element", "[Symbol]") {
  std::stringstream stream;
  stream << R"(for_range() { var s, x; s = 0; for(foo() : 0 .. 10 by 2) {s = s + x;} return s; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("foo() not an l-value"));
}

TEST_CASE("Check Assignable: binary expression as range-style for loop element", "[Symbol]") {
  std::stringstream stream;
  stream << R"(for_range() { var s, x; s = 0; for(x + 1 : 0 .. 10 by 2) {s = s + x;} return s; })";
  auto ast = ASTHelper::build_ast(stream);
  REQUIRE_THROWS_MATCHES(CheckAssignable::check(ast.get()), SemanticError,
                         ContainsWhat("(x+1) not an l-value"));
}

// End SIP extension
