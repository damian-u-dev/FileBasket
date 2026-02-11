#include "FileListModel.h"

#include <QFileInfo>
#include <QHash>
#include <QIcon>
#include <QFileIconProvider>

//NOTE: If Windows returns a file icon by its extension
//NOTE: but not by its content (for example, a .png image)
//NOTE: the hash can be optimized to store icons by file extension, rather than the path of each file.
QHash<QString, QIcon> iconCache;

FileListModel::FileListModel(AppModel& model, QObject* parent)
    : QAbstractListModel(parent)
    , model(model)
{

}


int FileListModel::rowCount(const QModelIndex &parent) const
{
    return model.activeTab().files.size();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return {};

    const auto& files = model.activeTab().files;

    if(index.row() < 0 || index.row() >= files.size())
        return {};

    const FileItem& item = files[index.row()];

    if(role == Qt::DisplayRole)
    {
        return QFileInfo(item.path).fileName();
    }

    if(role == Qt::ToolTipRole)
    {
        return item.path;
    }

    if(role == Qt::DecorationRole)
    {
        auto it = iconCache.find(item.path);
        if(it != iconCache.end())
            return it.value();

        static QFileIconProvider iconProvider;
        QIcon icon = iconProvider.icon(QFileInfo(item.path));
        iconCache.insert(item.path, icon);
        return icon;
    }

    return {};
}

void FileListModel::addFiles(const QStringList& paths)
{
    int before = model.activeTab().files.size();
    model.addFilesToActiveTab(paths);

    int after = model.activeTab().files.size();
    if(after > before)
    {
        beginInsertRows(QModelIndex(), before, after - 1);
        endInsertRows();
    }
}

void FileListModel::onFilesAdded(int startIndex, int count)
{
    beginInsertRows(QModelIndex(), startIndex, startIndex + count - 1);
    endInsertRows();
}

void FileListModel::onFilesRemoved(const QVector<int>& rows)
{
    if(rows.isEmpty())
        return;

    QVector<int> sorted;
    std::sort(sorted.begin(), sorted.end(), std::greater<>());

    for(int row : sorted)
    {
        beginRemoveRows(QModelIndex(), row, row);
        endRemoveRows();
    }
}
