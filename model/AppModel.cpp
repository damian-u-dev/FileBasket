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

    QSet<QString> existingPaths;
    for(const FileItem& item : std::as_const(tab.files))
        existingPaths.insert(item.path);

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
    }
}
