#include "ExceptionContainsWhat.h"
#include "FrontEnd.h"
#include "ParseError.h"
#include "ParserHelper.h"

#include <catch2/catch_test_macros.hpp>

#include "catch2/internal/catch_stdstreams.hpp"


// Boolean types and operators

TEST_CASE("SIP Parser: boolean types and operators", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      bools() {
        var x,y,z;
        x = true;
        y = false;
        z = x and y or y and x;
        z = not x or not y and not x or not z;
        z = not z and false or true;
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

// Array constructors and operators

TEST_CASE("SIP Parser: array declarations, element reference, and length", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      base_arr() {
        var x,y,z,a,b;
        a = [x,y,z];
        x = a[1];
        a = [y,z];
        a = [z];
        a = [];
        b = [y of z];
        z = #b;
        z = #[4, 5, 6];
        z = #[];
        return a;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: multi-dimensional array declarations and element reference", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      base_arr_multidim() {
        var x, y, z, a, b, s;
        x = [[0,0],[1,2],[2,3,4,6]];
        s = x[2][1];
        y = [[1 of 2], [3 of 4], [5 of 6]];
        z = [[1 of 2], [3,4], [5 of 6], [7,8]];
        a = [3 of [1,2]];
        b = [4 of [5 of 6]];
        return b;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

// Negation and modulo operators

TEST_CASE("SIP Parser: negation", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      neg() {
        var x, y, z;
        x = -x;
        y = - -x;
        z = - - -y;
        return z;
      }
    )";
  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: modulo", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      mod() {
        var x;
        x = x % 4;
        x = 4 % x;
        return x;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

// Comparison operators

TEST_CASE("SIP Parser: LTE", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      operators() {
        var x,y,z;
        if (x <= y) {
          while (y <= z) {
            y = y + 1;
          }
        }
        return x <= z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: GTE", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      operators() {
        var x,y,z;
        if (x >= y) {
          while (y >= z) {
            y = y + 1;
          }
        }
        return x >= z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: EQ", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      operators() {
        var x,y,z;
        if (x == y) {
          while (y == z) {
            y = y + 1;
          }
        }
        return x == z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

// Ternary operator

TEST_CASE("SIP Parser: ternary operator and nested ternary operators", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      ternary() {
        var x,a,b,c,d,e;
        x = a ? b : c;
        x = a ? b ? c : d : e;
        x = a ? b : c ? d : e;
        return x;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

// Increment and Decrement statements

TEST_CASE("SIP Parser: increment and decrement", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      operators() {
        var x, y;
        x++;
        y--;
        return x + y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

// Iterator-style for-loop

TEST_CASE("SIP Parser: for iterator", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      for_it() {
        var x,y,a;
        for (x : a) {
          y = y + 1;
        }
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: nested for iterator", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      for_it() {
        var x,y,z,a;
        for (x : a) {
          for (y : a) {
            z = z + 1;
          }
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

// Range-style for-loop

TEST_CASE("SIP Parser: for range statement", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      for_range() {
        var x,y,z;
        for (x : 0 .. y) {
          z = z - 1;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: for range statement w/ 'by'", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      for_range() {
        var x,y,z;
        for (x : 0 .. y by 2) {
          z = z - 1;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: nested for range", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      for_range() {
        var x,y,z;
        for (x : 0 .. y by 2) {
          for (y : 0 .. z) {

          z = z - 1;
          }
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: nested for iterator and range", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      for_nested() {
        var x,y,z,a;
        for (x : 0 .. y by 2) {
          for (y : a) {
            for (z : x .. 10) {
              z = z - 1;
            }
          }
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

/************ The following test operator precedence ************/

// Boolean operators

TEST_CASE("SIP Parser: and higher precedence than or", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(bool_op() { var x,y,z; return x or y and z; })";
  std::string expected = "(expr (expr x) or (expr (expr y) and (expr z)))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: not higher precedence than or", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(bool_op() { var x,y; return not x or not y; })";
  std::string expected = "(expr (expr not (expr x)) or (expr not (expr y)))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: not higher precedence than and", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(bool_op() { var x,y; return not x and not y; })";
  std::string expected = "(expr (expr not (expr x)) and (expr not (expr y)))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

// Negation and modulo operators

TEST_CASE("SIP Parser: negation higher precedence than addition/subtraction", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(neg() { var x,y,z; return -x + -y - -z; })";
  std::string expected = "(expr (expr (expr - (expr x)) + (expr - (expr y))) - (expr - (expr z)))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: negation higher precedence than multiplication/division", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(neg() { var x,y,z; return -x * -y / -z; })";
  std::string expected = "(expr (expr (expr - (expr x)) * (expr - (expr y))) / (expr - (expr z)))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: negation higher precedence than modulo", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(neg() { var x,y; return -x % -y; })";
  std::string expected = "(expr (expr - (expr x)) % (expr - (expr y)))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: modulo same precedence as multiplication/division", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(mod() { var a,b,c,d; return a * b % c / d; })";
  std::string expected = "(expr (expr (expr (expr a) * (expr b)) % (expr c)) / (expr d))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("TIP Parser: modulo higher precedence than addition", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(main() { return 1 + 2 % 3; })";
  std::string expected = "(expr (expr 1) + (expr (expr 2) % (expr 3)))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

// Ternary operator

TEST_CASE("TIP Parser: nested ternary operator precedence", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(main() { var a,b,c,d; return a ? b ? c : d : e; })";
  std::string expected = "(expr (expr a) ? (expr (expr b) ? (expr c) : (expr d)) : (expr e))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

/************ The following are expected to fail parsing ************/

// Boolean types and operators

TEST_CASE("SIP Parser: repeated boolean literals", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x; x = true false; return x; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: repeated boolean operators", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x,y,z; x = y and or z; return x; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

// Array constructors and operators

TEST_CASE("SIP Parser: repeated commas in base array constructor", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x,y,z,a; a = [x, y,, z]; return a; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: missing comma in base constructor", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x,y,z,a; a = [x, y z]; return a; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: missing 'of' in 'of' array constructor", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x,y,a; a = [x y]; return a; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array element reference with empty index", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x,y,z,a; a = [x, y, z]; return a[]; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

// Modulo operator

TEST_CASE("SIP Parser: repeated modulo", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x,y,z; z = x % % y; return z; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

// Ternary operator

TEST_CASE("SIP Parser: missing '?' in ternary operator", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x,y,z; return x y : z; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: missing ':' in ternary operator", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x,y,z; return x ? y z; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

// Increment and decrement statements

TEST_CASE("SIP Parser: increment missing '+'", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x; return x+; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: decrement missing '-'", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x; return x-; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: increment with extra '+'", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x; return x+++; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: decrement with extra '-'", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() { var x; return x---; }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

// Iterator-style for-loop

TEST_CASE("SIP Parser: for iterator missing ':'", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() {
        var x, i;
        x = 0;
        for (i [1, 2, 3]) {
          x = x + i;
        }
        return x;
      }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: for iterator without following statement", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() {
        var x, i;
        for (i : [1, 2, 3]);
        return x;
      }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

// Range-style for-loop

TEST_CASE("SIP Parser: for range missing ':'", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() {
        var i,x;
        for (i 0 .. 10 by 2) {
          x = x + 1;
        }
        return x;
      }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: for range missing '..'", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() {
        var i,x;
        for (i : 0 10 by 2) {
          x = x + 1;
        }
        return x;
      }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: for range blank 'by'", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() {
        var i,x;
        for (i : 0 .. 10 by) {
          x = x + 1;
        }
        return x;
      }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: for range without following statement", "[SIP Parser]") {
  std::stringstream stream;
  stream << R"(
      main() {
        var i,x;
        for (i : 0 .. 10);
        return x;
      }
    )";

  REQUIRE_FALSE(ParserHelper::is_parsable(stream));
}
