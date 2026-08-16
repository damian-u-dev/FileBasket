#ifndef FILEBASKETCONTROLLER_H
#define FILEBASKETCONTROLLER_H

#include <QObject>

class AppModel;
class FileOperationService;

class FileBasketController : public QObject
{
private:
    AppModel& model;
    FileOperationService& operationService;
public:
    FileBasketController(AppModel& model, FileOperationService& opera, QObject* parent = nullptr);
    void removeItems(const QVector<int>& indices);
    void moveItems(const QVector<int>& indices);
    bool createTab(const QString& name);
    bool renameTab(int index, const QString& newName);
    bool deleteTab(int index);
    void deleteTabs();
    void openFiles(const QVector<int>& indices);

public slots:
    void addFiles(const QStringList& paths);
    void copyTo(const QString& targetDir, const QVector<int>& selectedIndices);
    void moveTo(const QString& targetDir, const QVector<int>& selectedIndices);
    void setActiveTab(int index);
    void handleDrop(const QStringList& paths);

private:
    void handleDirectoryDrop(const QString& path);
    QStringList scanDirectory(const QString& path);
};

#endif // FILEBASKETCONTROLLER_H
