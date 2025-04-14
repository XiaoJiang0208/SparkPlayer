#include "SparkAIAPI.h"

Base64::Base64()
{
}

Base64::~Base64()
{
}

std::string Base64::base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

bool Base64::is_base64(const char c)
{    
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string Base64::Encode(const char *bytes_to_encode, unsigned int in_len)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
 
    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if(i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for(i = 0; (i <4) ; i++)
            {
                ret += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }
    if(i)
    {
        for(j = i; j < 3; j++)
        {
            char_array_3[j] = '\0';
        }
 
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
 
        for(j = 0; (j < i + 1); j++)
        {
            ret += base64_chars[char_array_4[j]];
        }
 
        while((i++ < 3))
        {
            ret += '=';
        }
 
    }
    return ret;
}

std::string Base64::Decode(const std::string &encoded_string)
{
    int in_len = (int) encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;
 
    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
 
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
 
            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;
 
        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);
 
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
 
        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }
 
    return ret;
}

std::string Base64::EncodeFile(const std::string &filePath)
{
    std::fstream f;
    f.open(filePath, std::ios::in|std::ios::binary);
    f.seekg(0, std::ios_base::end);     //设置偏移量至文件结尾
    std::streampos sp = f.tellg();      //获取文件大小
    int size = sp;
 
    char* buffer = (char*)malloc(sizeof(char)*size);
    f.seekg(0, std::ios_base::beg);     //设置偏移量至文件开头
    f.read(buffer,size);                //将文件内容读入buffer
 
    return Encode(buffer, size);         //编码
}

void Base64::DecodeFile(const std::string &base64,const std::string& filePath)
{
    std::string imgdecode64 = Decode(base64);          //解码
    //cout << "img decode size:" << imgdecode64.size() << endl;
 
    const char *p = imgdecode64.c_str();
    std::ofstream fout(filePath, std::ios::out|std::ios::binary);
    if (!fout)
    {
        //std::cout << "error" << endl;
    }
    else
    {
        //cout << "Success!" << endl;
        fout.write(p, imgdecode64.size());
    }
 
    fout.close();
}



SparkAIAPI::SparkAIAPI()
{
    curl_global_init(CURL_GLOBAL_ALL);
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl = curl_easy_init();    // 初始化
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // 设置请求头
    }
}

SparkAIAPI::~SparkAIAPI()
{
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

size_t SparkAIAPI::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string SparkAIAPI::httpGet(const std::string &url){
    std::string response;

    if (!curl) {
        //std::cerr << "curl_easy_init() failed" << std::endl;
        return "curl_easy_init() failed";
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        //std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        return std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res);
    }

    return response;
}

std::string SparkAIAPI::httpPost(const std::string &url, const std::string &data)
{
    std::string response;

    if (!curl) {
        //std::cerr << "curl_easy_init() failed" << std::endl;
        return "curl_easy_init() failed";
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        //std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        return std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res);
    }

    return response;
}

std::string SparkAIAPI::FaceAPIEmotion(const char *bytes_to_encode, unsigned int in_len)
{
    std::string base64img = Base64::Encode(bytes_to_encode,in_len);
    std::time_t now = std::time(nullptr);
    std::string time_str = std::to_string(now)+".png";
    QJsonObject feature = {
        {"animation", "no"},
        {"face","yes"}
    };
    QJsonObject data = {
        {"image", base64img.c_str()},
        {"data_type", "image"},
        {"file_name", time_str.c_str()},
        {"region_id", "cn-shanghai"},
        {"feature", feature}        
    };
    QJsonDocument doc(data);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    std::string response = httpPost("http://sakuraspark.fun:5090/upload",jsonString.toStdString());
    return response;
}

std::string SparkAIAPI::FaceAPIAnimation(const char *bytes_to_encode, unsigned int in_len)
{
    std::string base64img = Base64::Encode(bytes_to_encode,in_len);
    std::time_t now = std::time(nullptr);
    std::string time_str = std::to_string(now)+".png";
    QJsonObject feature = {
        {"animation", "yes"},
        {"face","no"}
    };
    QJsonObject data = {
        {"image", base64img.c_str()},
        {"data_type", "image"},
        {"file_name", time_str.c_str()},
        {"region_id", "cn-shanghai"},
        {"feature", feature}        
    };
    QJsonDocument doc(data);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    std::string response = httpPost("http://sakuraspark.fun:5090/upload",jsonString.toStdString());
    return response;
}

std::string SparkAIAPI::MusicAPIStyle(const char *bytes_to_encode, unsigned int in_len, std::string format, int sample_rate, int channel_count)
{
    std::string base64img = Base64::Encode(bytes_to_encode,in_len);
    std::time_t now = std::time(nullptr);
    std::string time_str = std::to_string(now)+"."+format;
    QJsonObject audio_info = {
        {"format", format.c_str()},
        {"sample_rate", sample_rate},
        {"channel", channel_count}
    };
    QJsonObject feature = {
        {"forced_refresh", "no"}
    };
    QJsonObject data = {
        {"audio", base64img.c_str()},
        {"data_type", "audio"},
        {"file_name", time_str.c_str()},
        {"audio_info", audio_info},
        {"feature", feature}
    };
    QJsonDocument doc(data);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    qDebug() << "jsonString:" << jsonString;
    std::string response = httpPost("http://sakuraspark.fun:5090/upload",jsonString.toStdString());
    return response;
}

std::string SparkAIAPI::MusicAPIStyle(const std::string &filepath)
{
    OutAudioFrameSetting settings = Codec::getRawAudioSettings(filepath);
    // 打开文件
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        return "";
    }
    // 获取文件大小
    std::streampos size = file.tellg();
    file.seekg(0, std::ios::beg);
    // 分配内存
    char* buffer = new char[size];
    // 读取文件内容
    file.read(buffer, size);
    // 关闭文件
    file.close();
    return MusicAPIStyle(buffer, size, settings.format, settings.sample_rate, settings.channel_count);
}

std::list<std::string> SparkAIAPI::getStyleFromResponse(std::string style)
{
    std::list<std::string> styles;
    QJsonDocument json = QJsonDocument::fromJson(style.c_str());
    if (json.isNull()) {
        qDebug() << "json error";
        return styles;
    }
    QJsonArray array = json.object()["tags"].toArray();
    for (const auto &item : array) {
        styles.push_back(item.toString().toStdString());
    }
    return styles;
}
