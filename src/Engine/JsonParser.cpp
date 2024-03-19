#include "Common/pch.h"
#include "Engine/JsonParser.h"

JsonParser::JsonParser() : document(), allocator(document.GetAllocator())
{
    document.SetObject();
}

void JsonParser::Parse(const std::string &json)
{
    document.Parse(json.c_str());
}

void JsonParser::ParseFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        HLOG_ERROR("Failed to open file: %s\n", filename.c_str());
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    Parse(buffer.str());
}

void JsonParser::ParseStream(std::istream &stream)
{
    std::stringstream buffer;
    buffer << stream.rdbuf();
    Parse(buffer.str());
}

void JsonParser::ParseString(const std::string &json)
{
    Parse(json);
}

void JsonParser::Write(const std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "wb");
    if (fp == nullptr)
    {
        HLOG_ERROR("Failed to open file: %s\n", filename.c_str());
        return;
    }
    char writeBuffer[65536];
    rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
    document.Accept(writer);

    fclose(fp);
}

std::string JsonParser::WriteString()
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

void JsonParser::AddValue(const std::string &key, const std::string &value)
{
    rapidjson::Value k(key.c_str(), allocator);
    rapidjson::Value v(value.c_str(), allocator);
    document.AddMember(k, v, allocator);
}

void JsonParser::AddValue(const std::string &key, int value)
{
    rapidjson::Value k(key.c_str(), allocator);
    document.AddMember(k, rapidjson::Value(value).Move(), allocator);
}

void JsonParser::AddValue(const std::string &key, float value)
{
    rapidjson::Value k(key.c_str(), allocator);
    document.AddMember(k, rapidjson::Value(value).Move(), allocator);
}

void JsonParser::AddValue(const std::string &key, bool value)
{
    rapidjson::Value k(key.c_str(), allocator);
    document.AddMember(k, rapidjson::Value(value).Move(), allocator);
}

void JsonParser::RemoveValue(const std::string &key)
{
    document.RemoveMember(key.c_str());
}

void JsonParser::Clear()
{
    document.SetObject();
}