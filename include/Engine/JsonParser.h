#pragma once
#include "Common/pch.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
class JsonParser
{
public:
    JsonParser();
    void Parse(const std::string &json);
    void ParseFile(const std::string &filename);
    void ParseStream(std::istream &stream);
    void ParseString(const std::string &json);
    void Write(const std::string &filename);
    std::string WriteString();
    void AddKey(const std::string &key,const std::string &value);
    void AddValue(const std::string &key, int value);
    void AddValue(const std::string &key, float value);
    void AddValue(const std::string &key, bool value);
    void RemoveValue(const std::string &key);
    void GetKey(const std::string &key, std::string &value);
    void GetValue(const std::string &key, int &value);
    void GetValue(const std::string &key, float &value);
    void GetValue(const std::string &key, bool &value);
    void Clear();

    friend inline std::ostream &operator<<(std::ostream &stream, JsonParser &parser);

private:
    rapidjson::Document document;
    rapidjson::Document::AllocatorType &allocator;
};

inline std::ostream &operator<<(std::ostream &stream, JsonParser &parser)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    parser.document.Accept(writer);
    stream << buffer.GetString();
    return stream;
}

#ifdef MODULE_TEST
inline void TestJsonParser()
{
    JsonParser parser;
    parser.Clear();
    parser.AddValue("test", "test");
    std::cout << parser << std::endl;
    parser.AddValue("test2", 1);
    std::cout << parser << std::endl;
    parser.AddValue("test3", 1.0f);
    std::cout << parser << std::endl;
    parser.RemoveValue("test2");
    std::cout << parser << std::endl;
    parser.AddValue("test4", true);
    parser.Write("test.json");
    std::cout << parser << std::endl;
    parser.Clear();
    parser.ParseFile("test.json");
    std::cout << parser << std::endl;
}
#endif