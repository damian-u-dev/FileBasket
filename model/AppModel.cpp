#include "AppModel.h"

#include <QSet>
#include <QFileInfo>
#include <QDir>

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
    emit modelChanged();
}

void AppModel::addFilesToActiveTab(const QStringList& paths)
{
    if(paths.isEmpty())
        return;

    qInfo() <<"Adding new files...\n";
    qInfo("---------");

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
        item.size = info.size();

        qInfo() << QStringLiteral("Added: %1").arg(info.fileName());
        tab.files.push_back(item);
        existingPaths.insert(normalizedPath);
        addedCount++;
    }
    if(addedCount > 0)
    {
        emit filesAdded(startIndex, addedCount);
        emit modelChanged();
    }
    qInfo("---------\n");
    qInfo() << "Adding new files done";
    qInfo() << QStringLiteral("Total added files: %1").arg(addedCount);
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
    emit modelChanged();
}

void AppModel::updatePaths(const QVector<int>& rows, const QString& targetDir)
{
    if(rows.isEmpty())
        return;

    auto& files = activeTab().files;
    for(int row : rows)
    {
        if(row < 0 || row >= files.size())
            continue;

        QFileInfo info(files[row].path);
        QString newPath = QDir(targetDir).filePath(info.fileName());
        files[row].path = newPath;
    }

    emit filesUpdated(rows);
}
