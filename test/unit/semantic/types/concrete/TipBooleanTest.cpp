#include "TipBoolean.h"
#include "TipFunction.h"
#include "TipBoolean.h"
#include "TipInt.h"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

TEST_CASE("TipBoolean: test TipBoolean is a TipCons"
          "[tip_boolean]") {
  TipBoolean t;
  REQUIRE_FALSE(nullptr == dynamic_cast<TipCons *>(&t));
}

TEST_CASE("TipBoolean: test TipBoolean is a TipType"
          "[tip_boolean]") {
  TipBoolean t;
  REQUIRE_FALSE(nullptr == dynamic_cast<TipType *>(&t));
}

TEST_CASE("TipBoolean: test args is empty", "[tip_boolean]") {
  TipBoolean t;
  REQUIRE(t.getArguments().empty());
}

TEST_CASE("TipBoolean: test toString returns int", "[tip_boolean]") {
  TipBoolean t;
  std::stringstream stream;
  stream << t;
  REQUIRE("boolean" == stream.str());
}

TEST_CASE("TipBoolean: test all TipBooleans are equal", "[tip_boolean]") {
  TipBoolean t1;
  TipBoolean t2;
  TipInt t3;
  REQUIRE(t1 == t2);
  REQUIRE(t1 != t3);
}
