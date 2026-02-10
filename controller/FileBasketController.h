#ifndef FILEBASKETCONTROLLER_H
#define FILEBASKETCONTROLLER_H

#include <QObject>

class AppModel;

class FileBasketController : public QObject
{
private:
    AppModel& model;
public:
    FileBasketController(AppModel& model, QObject* parent = nullptr);
    void removeItems(const QVector<int>& indices);

public slots:
    void addFiles(const QStringList& paths);
    void copyTo(const QString& targetDir, const QVector<int>& selectedIndices);
    void moveTo(const QString& targetDir, const QVector<int>& selectedIndices);
    void setActiveTab(int index);
};

#endif // FILEBASKETCONTROLLER_H
