#ifndef SPARKAIAPI_H
#define SPARKAIAPI_H

#include <curl/curl.h>
#include <string>
#include <cstring>
#include <fstream>
#include <malloc.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include "utils/Codec.h"

class Base64
{
private:
    /* data */
public:
    Base64(/* args */);
    ~Base64();

    static std::string base64_chars;
    
    static bool is_base64(const char c);
    static std::string Encode(const char * bytes_to_encode, unsigned int in_len);
    static std::string Decode(const std::string& encoded_string);
    static std::string EncodeFile(const std::string& filePath);
    static void DecodeFile(const std::string& base64,const std::string& filePath);
};


class SparkAIAPI
{
private:
    CURL* curl = curl_easy_init();
    CURLcode res;
    struct curl_slist *headers = NULL;
    SparkAIAPI(/* args */);
public:
    ~SparkAIAPI();

    // 获取API实例
    static SparkAIAPI& getInstance()
    {
        static SparkAIAPI instance;
        return instance;
    }

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    std::string httpGet(const std::string& url);
    std::string httpPost(const std::string& url, const std::string& data);

    std::string FaceAPIEmotion(const char *bytes_to_encode, unsigned int in_len);
    std::string FaceAPIAnimation(const char *bytes_to_encode, unsigned int in_len);
    std::string MusicAPIStyle(const char *bytes_to_encode, unsigned int in_len, std::string format, int sample_rate, int channel_count);
    std::string MusicAPIStyle(const std::string& filepath);
    static std::list<std::string> getStyleFromResponse(std::string style);
};


#endif // SPARKAIAPI_H