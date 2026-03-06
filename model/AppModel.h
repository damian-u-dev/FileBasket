#ifndef APPMODEL_H
#define APPMODEL_H

#include <QString>
#include <QVector>
#include <QObject>

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

class AppModel : public QObject
{
private:
    Q_OBJECT
    QVector<Tab> tabs;
    int currentTab = 0;
public:
    AppModel();
    Tab& activeTab();
    qsizetype getSizeTabs() const;
    void setCurrentTab(int index);
    void addFilesToActiveTab(const QStringList& paths);
    void removeFilesFromActiveTab(const QVector<int>& rows);
    void updatePaths(const QVector<int>& rows, const QString& targetDir);
    int moveFilesFromActiveTab(QVector<int> rows, int indexAnotherTab);
    QStringList tabNames() const;
    int getCurrentTabIndex() const;
    QString getTabName(int index) const;
    int getTabIndexByName(const QString& tabName);
    bool createTab(const QString& name);

signals:
    void filesAdded(int startIndex, int count);
    void filesRemoved(const QVector<int>& rows);
    void filesUpdated(const QVector<int>& rows);
    void modelChanged();
    void currentTabChanged(int index);
    void tabsChanged();
};

#endif // APPMODEL_H
