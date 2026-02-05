#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "../model/AppModel.h"


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
};

#endif // FILELISTMODEL_H
