#include "SparkSettings.h"

SparkSettings::SparkSettings()
{
    QSettings *setting = new QSettings("SakuraSpark","SparkPlayer");
    setting->setIniCodec(QTextCodec::codecForName("UTF-8"));
}

SparkSettings::~SparkSettings()
{
}

SparkSettings *SparkSettings::getInstance()
{
    static SparkSettings instance;
    return &instance;
}

void SparkSettings::setMediaLibraries(std::list<fs::path> play_list)
{
    settings.remove("MediaLibraries");
    settings.beginGroup("MediaLibraries");
    int index = 0;
    for (auto path : play_list) {
        settings.setValue(QString(index),path.c_str());

    }
    settings.endGroup();
    settings.sync();
}

std::list<fs::path> SparkSettings::getMediaLibraries()
{
    std::list<fs::path> media_list;

    settings.beginGroup("MediaLibraries");
    for (auto key : settings.allKeys()) {
        media_list.push_back(fs::path(settings.value(key).toString().toStdString()));
    }
    settings.endGroup();

    return media_list;
}
