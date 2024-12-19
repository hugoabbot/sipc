#include "TipArray.h"
#include "TipFunction.h"
#include "TipInt.h"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

TEST_CASE("TipArray: Test TipArrays are compared by their underlying term"
          "[tip_array]") {
  auto term = std::make_shared<TipInt>();
  TipArray tipArray(term);
  REQUIRE(tipArray.getFieldType() == term);

  SECTION("Equal when terms are the same") {
    auto term = std::make_shared<TipInt>();
    TipArray tipArray2(term);
    REQUIRE(tipArray == tipArray2);
  }

  SECTION("Not equal when terms are different") {
    std::vector<std::shared_ptr<TipType>> params{std::make_shared<TipInt>(),
                                                 std::make_shared<TipInt>()};
    auto ret = std::make_shared<TipArray>(std::make_shared<TipInt>());
    auto tipFunction = std::make_shared<TipFunction>(params, ret);
    TipArray tipArray2(tipFunction);

    REQUIRE_FALSE(tipArray == tipArray2);
    REQUIRE(tipArray != tipArray2);
  }
}

TEST_CASE("TipArray: test TipArray is a TipCons"
          "[tip_array]") {
  auto term = std::make_shared<TipInt>();
  TipArray tipArray(term);
  REQUIRE_FALSE(nullptr == dynamic_cast<TipCons *>(&tipArray));
}

TEST_CASE("TipBoolean: test TipBoolean is a TipType"
          "[tip_array]") {
  auto term = std::make_shared<TipInt>();
  TipArray tipArray(term);
  REQUIRE_FALSE(nullptr == dynamic_cast<TipType *>(&tipArray));
}

TEST_CASE("TipArray: Test arity is one"
          "[tip_array]") {
  auto term = std::make_shared<TipInt>();
  TipArray tipArray(term);
  REQUIRE(1 == tipArray.arity());
}

TEST_CASE("TipArray: Test getter"
          "[tip_array]") {
  std::vector<std::shared_ptr<TipType>> params{std::make_shared<TipInt>(),
                                               std::make_shared<TipInt>()};
  auto ret = std::make_shared<TipArray>(std::make_shared<TipInt>());
  auto tipFunction = std::make_shared<TipFunction>(params, ret);
  TipArray tipArray(tipFunction);

  REQUIRE(*tipFunction == *tipArray.getFieldType());
}

TEST_CASE("TipArray: Test output stream"
          "[tip_array]") {
  std::vector<std::shared_ptr<TipType>> params{std::make_shared<TipInt>(),
                                               std::make_shared<TipInt>()};
  auto ret = std::make_shared<TipInt>();
  auto tipFunction = std::make_shared<TipFunction>(params, ret);
  TipArray tipArray(tipFunction);

  auto expectedValue = "array[(int,int) -> int]";
  std::stringstream stream;
  stream << tipArray;
  std::string actualValue = stream.str();

  REQUIRE(expectedValue == actualValue);

  auto intType = std::make_shared<TipInt>();
  TipArray tipArray2(intType);

  expectedValue = "array[int]";
  stream.str("");
  stream << tipArray2;
  actualValue = stream.str();

  REQUIRE(expectedValue == actualValue);
}
