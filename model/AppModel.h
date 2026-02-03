#ifndef APPMODEL_H
#define APPMODEL_H

#include <QString>
#include <QVector>

struct FileItem
{
    QString path;
    qint64 size = -1; //size of file
};

struct Tab
{
    QString name;
    QVector<FileItem> files;
};

class AppModel
{
private:
    QVector<Tab> tabs;
    int currentTab = 0;
public:
    AppModel();
    Tab& activeTab();
};

#endif // APPMODEL_H
