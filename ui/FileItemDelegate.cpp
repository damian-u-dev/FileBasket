#include "FileItemDelegate.h"
#include "../model/FileListModel.h"

#include <QPainter>
#include <QApplication>

FileItemDelegate::FileItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{}

void FileItemDelegate::paint(QPainter* painter,
                             const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    painter->save();

    QString name  = index.data(Qt::DisplayRole).toString();
    QString path  = index.data(FileListModel::FilePathRole).toString();
    qint64  size  = index.data(FileListModel::FileSizeRole).toLongLong();

    QRect r = option.rect;

    bool selected = option.state & QStyle::State_Selected;
    auto pen = (selected ? option.palette.highlightedText().color() : option.palette.text().color());

    painter->setPen(pen);

    painter->drawText(r.adjusted(5, 5, -5, -25), name);

    pen = (selected ? option.palette.highlightedText().color() : QColor(120, 120, 120));
    painter->setPen(pen);

    painter->drawText(r.adjusted(5, 25, -5, -5), path + " | " + QString::number(size) + " bytes");

    painter->restore();
}

QSize FileItemDelegate::sizeHint(const QStyleOptionViewItem& option,
               const QModelIndex& index) const
{
    return QSize(200, 50);
}
