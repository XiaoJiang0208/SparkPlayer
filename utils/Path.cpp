#include "Path.h"

Path::Path(const QString &path)
{
    this->path = path;
}

Path Path::applicationPath()
{
    return Path(QCoreApplication::applicationDirPath());
}
Path Path::applicationPath(const QString &path)
{
    return applicationPath().addPath(path);
}

Path Path::currentPath()
{
    return Path(QDir::currentPath());
}
Path Path::currentPath(const QString &path)
{
    return currentPath().addPath(path);
}

Path Path::addPath(const QString &path)
{
    this->path = QDir::cleanPath(this->path+QDir::separator()+path);
    return *this;
}

QString Path::toString()
{
    return path;
}
