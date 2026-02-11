#include "AppModel.h"

#include <QSet>
#include <QFileInfo>

AppModel::AppModel()
{
    tabs.push_back({"Default", {}}); //TODO: PlaceHolder;
}

Tab& AppModel::activeTab()
{
    return tabs[currentTab];
}

qsizetype AppModel::getSizeTabs() const
{
    return tabs.size();
}

void AppModel::setCurrentTab(int index)
{
    if(index < 0 || index > getSizeTabs())
        return;

    currentTab = index;
}

void AppModel::addFilesToActiveTab(const QStringList& paths)
{
    if(paths.isEmpty())
        return;

    Tab& tab = activeTab();

    int startIndex = tab.files.size();

    QSet<QString> existingPaths;
    for(const FileItem& item : std::as_const(tab.files))
        existingPaths.insert(item.path);

    int addedCount = 0;

    for(const QString& rawPath : paths)
    {
        QFileInfo info(rawPath);
        if(!info.exists())
            continue;

        QString normalizedPath = info.absoluteFilePath();
        if(existingPaths.contains(normalizedPath))
            continue;

        FileItem item;
        item.path = normalizedPath;
        item.size = -1;

        tab.files.push_back(item);
        existingPaths.insert(normalizedPath);
        addedCount++;
    }
    if(addedCount > 0)
    {
        emit filesAdded(startIndex, addedCount);
    }
}

void AppModel::removeFilesFromActiveTab(const QVector<int>& rows)
{
    if(rows.isEmpty())
        return;

    auto& files = activeTab().files;

    //Start from the end because
    //The array elements will be
    //Shifted by -1 after removing
    QVector<int> sorted = rows;
    std::sort(sorted.begin(), sorted.end(), std::greater<>());

    for(int row : sorted)
        if(row >= 0 && row < files.size())
            files.removeAt(row);

    emit filesRemoved(rows);
}
