#ifndef SPARKSETTINGS_H
#define SPARKSETTINGS_H

#include <QSettings>
#include <QCoreApplication>
#include <QTextCodec>
#include <filesystem>
#include <QDebug>

namespace fs = std::filesystem;

class SparkSettings
{
private:
    QSettings settings;

    SparkSettings(/* args */);

public:
    ~SparkSettings();

    static SparkSettings *getInstance();

    void setMediaLibraries(std::list<fs::path> play_list);
    std::list<fs::path> getMediaLibraries();
};

#endif // SPARKSETTINGS_H