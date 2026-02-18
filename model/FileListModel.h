#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include "../model/AppModel.h"

#include <QAbstractListModel>
#include <QStringList>
#include <QVector>


class FileListModel : public QAbstractListModel
{
private:
    Q_OBJECT
    AppModel& model;

public:
    FileListModel(AppModel& model, QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void addFiles(const QStringList& paths);

public slots:
    void onFilesAdded(int startIndex, int count);
    void onFilesRemoved(const QVector<int>& rows);
    void onFilesUpdated(const QVector<int>& rows);
};

#endif // FILELISTMODEL_H
