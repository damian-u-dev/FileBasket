#include "FileItemDelegate.h"
#include "../model/FileListModel.h"

#include <QPainter>

FileItemDelegate::FileItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{}

void FileItemDelegate::paint(QPainter* painter,
                             const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{
    painter->save();

    QString name  = index.data(Qt::DisplayRole).toString();
    QString path  = index.data(FileListModel::FilePathRole).toString();
    qint64  size  = index.data(FileListModel::FileSizeRole).toLongLong();

    QRect r = option.rect;
    painter->drawText(r.adjusted(5, 5, -5, -25), name);
    painter->drawText(r.adjusted(5, 25, -5, -5), path + " | " + QString::number(size) + " bytes");

    painter->restore();
}

QSize FileItemDelegate::sizeHint(const QStyleOptionViewItem& option,
               const QModelIndex& index) const
{
    return QSize(200, 50);
}
