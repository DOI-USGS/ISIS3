#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "TestUtilities.h"

using json = nlohmann::json;
using namespace Isis;
using namespace std;

TEST(AssertJsonsNear, BasicComparisons) {
  json testJson1 = {
      {"pi", 3.14},
      {"array", {1, 0, 2}},
      {"nested_array", {{1, 2}, {3, 4}, {5, 6}}},
      {"object", {
        {"one", 1},
        {"two", "2"}
      }}
  };
  EXPECT_PRED_FORMAT3(AssertJsonsNear, testJson1, testJson1, 1);

  json testJson2 = testJson1;
  testJson2["new_value"] = "new";
  EXPECT_FALSE(AssertJsonsNear("json1", "json2", "tolerance", testJson1, testJson2, 1));
  EXPECT_FALSE(AssertJsonsNear("json2", "json1", "tolerance", testJson2, testJson1, 1));

  json testJson3 = testJson1;
  testJson3["pi"] = "3.14";
  EXPECT_FALSE(AssertJsonsNear("json1", "json3", "tolerance", testJson1, testJson3, 1));

  json testJson4 = testJson1;
  testJson4["array"].push_back(3);
  EXPECT_FALSE(AssertJsonsNear("json1", "json4", "tolerance", testJson1, testJson4, 1));

  json testJson5 = testJson1;
  testJson5["object"]["one"] = "1";
  EXPECT_FALSE(AssertJsonsNear("json1", "json5", "tolerance", testJson1, testJson5, 1));

  json testJson6 = testJson1;
  testJson6["nested_array"][1].push_back(-1);
  EXPECT_FALSE(AssertJsonsNear("json1", "json6", "tolerance", testJson1, testJson6, 1));
}


TEST(AssertJsonsNear, Tolerance) {
  json testJson1 = {
      {"pi", 3.14},
      {"array", {1, 0, 2}},
      {"nested_array", {{1, 2}, {3, 4}, {5, 6}}},
      {"object", {
        {"one", 1},
        {"two", "2"}
      }}
  };

  json testJson2 = testJson1;
  testJson2["pi"] = 3;
  EXPECT_TRUE(AssertJsonsNear("json1", "json2", "1", testJson1, testJson2, 1));
  EXPECT_FALSE(AssertJsonsNear("json1", "json2", "0.1", testJson1, testJson2, 0.1));

  json testJson3 = testJson1;
  testJson3["nested_array"][2][1] = 5.5;
  EXPECT_TRUE(AssertJsonsNear("json1", "json3", "1", testJson1, testJson3, 1));
  EXPECT_FALSE(AssertJsonsNear("json1", "json3", "0.1", testJson1, testJson3, 0.1));

  json testJson4 = testJson1;
  testJson4["object"]["one"] = 0.5;
  EXPECT_TRUE(AssertJsonsNear("json1", "json3", "1", testJson1, testJson4, 1));
  EXPECT_FALSE(AssertJsonsNear("json1", "json3", "0.1", testJson1, testJson4, 0.1));
}