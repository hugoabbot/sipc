#include "PrettyPrinter.h"
#include "ASTHelper.h"
#include "GeneralHelper.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

TEST_CASE("SIPPrettyPrinterTest: ternary expression", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog() { var x, y, z, a; a=x?y:z; return a; })";

  std::string expected = R"(prog() 
{
  var x, y, z, a;
  a = x ? y : z;
  return a;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: increment statement", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog() { var x; x++; return x; })";

  std::string expected = R"(prog() 
{
  var x;
  x++;
  return x;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: decrement statement", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog() { var x; x--; return x; })";

  std::string expected = R"(prog() 
{
  var x;
  x--;
  return x;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: boolean expression (both true and false)", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog() { var x; x=true; x=false; return x; })";

  std::string expected = R"(prog() 
{
  var x;
  x = true;
  x = false;
  return x;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: default array", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog() { var x,y; x=[1,y,89]; return x; })";

  std::string expected = R"(prog() 
{
  var x, y;
  x = [1, y, 89];
  return x;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: default array empty", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog() { var x; x=[]; return x; })";

  std::string expected = R"(prog()
{
  var x;
  x = [];
  return x;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: fixed array", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog() { var x,y; x=9; y=[x of 3]; return x; })";

  std::string expected = R"(prog() 
{
  var x, y;
  x = 9;
  y = [x of 3];
  return x;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: unary not", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog() { var x,y; x=not y; return x; })";

  std::string expected = R"(prog() 
{
  var x, y;
  x = not (y);
  return x;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: Test iterator-style for print", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog(a) { var s, x; s = 0; for(x : a) s = s + x; return s; })";

  std::string expected = R"(prog(a)
{
  var s, x;
  s = 0;
  for (x : a)
    s = (s + x);
  return s;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: Test range-style for without 'by' print", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog(a) { var s, x; s = 0; for(x : 0 .. y) s = s + x; return s; })";

  std::string expected = R"(prog(a)
{
  var s, x;
  s = 0;
  for (x : 0 .. y by 1)
    s = (s + x);
  return s;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: Test range-style for with 'by' print", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog(a, z) { var s, x; s = 0; for(x : 0 .. y by z) s = s + x; return s; })";

  std::string expected = R"(prog(a, z)
{
  var s, x;
  s = 0;
  for (x : 0 .. y by z)
    s = (s + x);
  return s;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}


TEST_CASE("SIPPrettyPrinterTest: Test array length and reference operators print", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog(x, i) { var a; a = [x of 1]; return #a + a[i]; })";

  std::string expected = R"(prog(x, i)
{
  var a;
  a = [x of 1];
  return (#a + a[i]);
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: Test negated expressions print", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog(x) { var y; y = -(x + 3); return - -y; })";

  std::string expected = R"(prog(x)
{
  var y;
  y = -((x + 3));
  return -(-(y));
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: Test 'and' and 'or' expressions print", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog(x, y) { var a, b; a = x and true; b = false or y; return a and b; })";

  std::string expected = R"(prog(x, y)
{
  var a, b;
  a = (x and true);
  b = (false or y);
  return (a and b);
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: Test LT, LTE, and GTE expressions print", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog(x) { var y; if (x < 2) y = 1; if (x <= 1) y = 2; if (x >= 2) y = 3; return y; })";

  std::string expected = R"(prog(x)
{
  var y;
  if ((x < 2))
    y = 1;
  if ((x <= 1))
    y = 2;
  if ((x >= 2))
    y = 3;
  return y;
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

TEST_CASE("SIPPrettyPrinterTest: Test mod expressions print", "[SIPPrettyPrinter]") {
  std::stringstream stream;
  stream << R"(prog(x) { return 42 % x; })";

  std::string expected = R"(prog(x)
{
  return (42 % x);
}
)";

  std::stringstream pp;
  auto ast = ASTHelper::build_ast(stream);
  PrettyPrinter::print(ast.get(), pp, ' ', 2);
  std::string ppString = GeneralHelper::removeTrailingWhitespace(pp.str());
  expected = GeneralHelper::removeTrailingWhitespace(expected);
  REQUIRE(ppString == expected);
}

