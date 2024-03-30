#pragma once
#include <gtest/gtest.h>
#include "Engine/JsonParser.h"
#include <sstream>

class JsonParserTest : public ::testing::Test {
protected:
    JsonParser parser;

    void SetUp() override {
        // 在这里可以进行一些初始设置
    }

    void TearDown() override {
        // 测试结束后的清理工作
    }

    bool JSONEquals(const std::string& json1, const std::string& json2) {
        rapidjson::Document d1;
        d1.Parse(json1.c_str());
        rapidjson::Document d2;
        d2.Parse(json2.c_str());
        return d1 == d2;
    }

    bool FloatAlmostEqual(float a, float b, float epsilon = 0.001f) {
        return std::fabs(a - b) < epsilon;
    }

};

TEST_F(JsonParserTest, ParseString) {
    std::string json = R"({"key":"value"})";
    parser.ParseString(json);
    std::string result = parser.WriteString();
    EXPECT_TRUE(JSONEquals(result, json));
}

TEST_F(JsonParserTest, AddValueString) {
    parser.Clear();
    parser.AddKey("key", "value");
    std::string result = parser.WriteString();
    EXPECT_TRUE(JSONEquals(result, R"({"key":"value"})"));
}

TEST_F(JsonParserTest, AddValueInt) {
    parser.Clear();
    parser.AddValue("number", 123);
    std::string result = parser.WriteString();
    EXPECT_TRUE(JSONEquals(result, R"({"number":123})"));
}

TEST_F(JsonParserTest, AddValueFloat) {
    parser.Clear();
    parser.AddValue("float", 1.23f);
    float result = 0;
    parser.GetValue("float", result);
    EXPECT_TRUE(FloatAlmostEqual(result, 1.23f));
}

TEST_F(JsonParserTest, AddValueBool) {
    parser.Clear();
    parser.AddValue("bool", true);
    std::string result = parser.WriteString();
    EXPECT_TRUE(JSONEquals(result, R"({"bool":true})"));
}