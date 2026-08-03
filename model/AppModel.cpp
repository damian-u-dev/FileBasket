#include "AppModel.h"
#include "../Config.h"

#include <QSet>
#include <QFileInfo>
#include <QDir>
#include <numeric>

AppModel::AppModel()
{
    watcher = new QFileSystemWatcher(this);
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &AppModel::onFileChanged);
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
    emit tabsChanged();
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

        if(!watcher->files().contains(rawPath))
            watcher->addPath(rawPath);

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

    //Note: Start from the end because
    //Note: The array elements will be
    //Note: Shifted by -1 after removing
    QVector<int> sorted = rows;
    std::sort(sorted.begin(), sorted.end(), std::greater<>());

    QStringList removedPaths;
    for(int row : sorted)
    {
        if(row >= 0 && row < files.size())
        {
            removedPaths.append(files[row].path);
            files.removeAt(row);
        }
    }

    for(const auto& path : removedPaths)
    {
        if(!isPathUsedInAnyTab(path))
        {
            watcher->removePath(path);
        }
    }

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

    if(!watcher->files().contains(path))
        watcher->addPath(path);

    tabs[index].files.push_back({path, size});
}

bool AppModel::renameTab(int index, const QString& newName)
{
    QString oldName = getNameActiveTab();

    if(oldName == newName)
        return false;

    if(!isNameTabUnique(newName))
        return false;

    if(index < 0 || index >= tabs.size())
        return false;

    tabs[index].name = newName;

    emit tabsChanged();
    return true;
}

bool AppModel::deleteTab(int index)
{
    if(index < 0 || index >= tabs.size())
    {
        return false;
    }
    else if(getAmountTabs() == 1)
    {
        clearActiveTab();
        renameTab(index, Config::DefaultTabName);
        return true;
    }
    else if(index == currentTab)
    {
        setActiveTab(currentTab - 1);
        tabs.removeAt(index);
    }
    else if(index != currentTab)
    {
        QString name = getNameActiveTab();
        tabs.removeAt(index);
        setActiveTab(getTabIndexByName(name));
    }

    emit tabsChanged();
    return true;
}

void AppModel::onFileChanged(const QString& path)
{
    if(!QFile::exists(path))
    {
        removePathGlobally(path);
    }
    else
    {
        updateFileByPath(path);
    }
}

void AppModel::removeFileByPath(const QString& path)
{
    auto& files = activeTab().files;
    for(int i = 0; i < files.size(); i++)
    {
        if(files[i].path == path)
        {
            files.remove(i);
            emit filesRemoved({i});
            break;
        }
    }

    if(!isPathUsedInAnyTab(path))
    {
        watcher->removePath(path);
    }
}

void AppModel::updateFileByPath(const QString& path)
{
    auto& files = activeTab().files;
    for(int i = 0; i < files.size(); i++)
    {
        if(files[i].path == path)
        {
            QFileInfo info(path);
            files[i].size = info.size();
            emit filesUpdated({i});
            return;
        }
    }
}

void AppModel::clearActiveTab()
{
    const int SIZE = activeTab().files.size();
    QVector<int> rows(SIZE);

    std::iota(rows.begin(), rows.end(), 0);

    activeTab().files.clear();

    emit filesRemoved(rows);
    emit modelChanged();
}

int AppModel::getIndexLastTab() const
{
    return getAmountTabs() - 1;
}

bool AppModel::isNameTabUnique(const QString& newName)
{
    for(const Tab& tab : std::as_const(tabs))
    {
        if(tab.name == newName)
            return false;
    }
    return true;
}

void AppModel::deleteTabs()
{
    for(int i = getIndexLastTab(); i >= 0; i--)
    {
        deleteTab(i);
    }
}

bool AppModel::isPathUsedInAnyTab(const QString& path) const
{
    for (const Tab& tab : tabs)
    {
        for (const FileItem& item : tab.files)
        {
            if (item.path == path)
                return true;
        }
    }
    return false;
}

void AppModel::removePathGlobally(const QString& path)
{
    bool modified = false;

    for (int t = 0; t < tabs.size(); t++)
    {
        auto& files = tabs[t].files;
        for (int i = files.size() - 1; i >= 0; i--)
        {
            if (files[i].path == path)
            {
                files.remove(i);
                modified = true;

                if (t == currentTab)
                {
                    //NOTE: Updating FileList for current tab
                    emit filesRemoved({i});
                }
                break;
            }
        }
    }

    //NOTE: Updating UI
    if (modified)
    {
        watcher->removePath(path);
        emit modelChanged();
    }
}
