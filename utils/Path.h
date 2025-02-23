#ifndef PATH_H
#define PATH_H

#include <QString>
#include <QCoreApplication>
#include <QDir>

class Path
{
private:
    QString path;
    Path(const QString &path);
public:
    static Path applicationPath();
    static Path applicationPath(const QString &path);
    static Path currentPath();
    static Path currentPath(const QString &path);
    Path addPath(const QString &path);
    QString toString();
};

#endif // PATH_H