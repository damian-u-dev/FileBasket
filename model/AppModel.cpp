#include "AppModel.h"

#include <QSet>
#include <QFileInfo>
#include <QDir>

AppModel::AppModel()
{
    persistence.load(*this);
}

AppModel::~AppModel()
{
    persistence.save(*this);
}

Tab& AppModel::activeTab()
{
    return tabs[currentTab];
}

qsizetype AppModel::getAmountTabs() const
{
    return tabs.size();
}

void AppModel::setActiveTab(int index)
{
    if(index < 0 || index > getAmountTabs())
        return;

    if(index == currentTab)
        return;

    currentTab = index;

    emit activeTabChanged(index);
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

int AppModel::moveFilesFromActiveTab(QVector<int> rows, int indexAnotherTab)
{
    if(indexAnotherTab < 0 || indexAnotherTab >= tabs.size())
        return 0;

    if(rows.isEmpty())
        return 0;

    auto& sourceTab = tabs[currentTab].files;
    auto& targetTab = tabs[indexAnotherTab].files;

    std::sort(rows.begin(), rows.end(), std::greater<>());

    QSet<QString> existingPathsTarget;
    for(const FileItem& item : targetTab)
        existingPathsTarget.insert(item.path);

    int totalMoved = 0;

    QVector<FileItem> filesMove;

    for(int row : rows)
    {
        if(row < 0 || row >= sourceTab.size())
            continue;

        const QString& path = sourceTab[row].path;

        if(existingPathsTarget.contains(path))
            continue;

        filesMove.push_back(std::move(sourceTab[row]));
        sourceTab.removeAt(row);

        totalMoved++;
    }

    for(auto it = filesMove.rbegin(); it != filesMove.rend(); it++)
    {
        targetTab.push_back(*it);
    }

    emit filesRemoved(rows);
    emit modelChanged();

    return totalMoved;
}

QStringList AppModel::getTabNames() const
{
    QStringList names;
    for(const Tab& tab : tabs)
        names.append(tab.name);
    return names;
}

int AppModel::getIndexActiveTab() const
{
    return currentTab;
}

QString AppModel::getTabName(int index) const
{
    if(index >= 0 && index < getAmountTabs())
        return tabs[index].name;
    return QString();
}

int AppModel::getTabIndexByName(const QString& tabName)
{
    for(int i = 0; i < getAmountTabs(); i++)
    {
        if(getTabName(i) == tabName)
            return i;
    }
    return -1;
}

bool AppModel::createTab(const QString& name)
{
    for(const Tab& tab : std::as_const(tabs))
    {
        if(tab.name == name)
            return false;
    }

    tabs.push_back({name, {}});
    emit tabsChanged();

    return true;
}

bool AppModel::isAppModelEmpty() const
{
    return tabs.isEmpty();
}

QString AppModel::getNameActiveTab() const
{
    return getTabName(currentTab);
}

const QVector<Tab>& AppModel::getTabs() const
{
    return tabs;
}

void AppModel::addFileToTab(const QString& tab, const QString& path, qint64 size)
{
    int index = getTabIndexByName(tab);
    tabs[index].files.push_back({path, size});
}

bool AppModel::renameTab(int index, const QString& newName)
{
    if(index < 0 || index >= tabs.size())
        return false;

    tabs[index].name = newName;

    emit tabsChanged();
    return true;
}

bool AppModel::deleteTab(int index)
{
    if(index < 0 || index >= tabs.size())
        return false;

    if(index == currentTab)
    {
        setActiveTab(currentTab - 1);
    }

    tabs.removeAt(index);

    emit tabsChanged();
    return true;
}
