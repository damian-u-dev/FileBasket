#ifndef APPMODEL_H
#define APPMODEL_H

#include "../services/PersistenceService.h"

#include <QString>
#include <QVector>
#include <QObject>
#include <QFileSystemWatcher>

struct FileItem
{
    QString path;
    qint64 size = -1;
};

struct Tab
{
    QString name;
    QVector<FileItem> files;
};

class AppModel : public QObject
{
private:
    Q_OBJECT
    QVector<Tab> tabs;
    int currentTab = 0;
    PersistenceService persistence;
    QFileSystemWatcher* watcher;

public:
    AppModel();
    ~AppModel();
    Tab& activeTab();
    qsizetype getAmountTabs() const;
    void setActiveTab(int index);
    void addFilesToActiveTab(const QStringList& paths);
    void removeFilesFromActiveTab(const QVector<int>& rows);
    void updatePaths(const QVector<int>& rows, const QString& targetDir);
    int moveFilesFromActiveTab(QVector<int> rows, int indexAnotherTab);
    QStringList getTabNames() const;
    int getIndexActiveTab() const;
    QString getTabName(int index) const;
    int getTabIndexByName(const QString& tabName);
    bool createTab(const QString& name);
    bool isAppModelEmpty() const;
    QString getNameActiveTab() const;
    const QVector<Tab>& getTabs() const;
    void addFileToTab(const QString& tab, const QString& path, qint64 size);
    bool renameTab(int index, const QString& newName);
    bool deleteTab(int index);

private:
    void removeFileByPath(const QString& path);
    void updateFileByPath(const QString& path);

signals:
    void filesAdded(int startIndex, int count);
    void filesRemoved(const QVector<int>& rows);
    void filesUpdated(const QVector<int>& rows);
    void modelChanged();
    void activeTabChanged(int index);
    void tabsChanged();

private slots:
    void onFileChanged(const QString& path);
};

#endif // APPMODEL_H
