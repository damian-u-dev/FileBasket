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

    QStyle* style = opt.widget
        ? opt.widget->style()
        : QApplication::style();
    
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    painter->save();

    QIcon icon     = index.data(Qt::DecorationRole).value<QIcon>();
    QSize iconSize = opt.decorationSize;

    QRect iconRect = QRect(
    opt.rect.left() + 5,
    opt.rect.top() + (opt.rect.height() - iconSize.height()) / 2,
    iconSize.width(),
    iconSize.height()
    );

    icon.paint(painter, iconRect);

    int textLeft = iconRect.right() + 10;

    QString name  = index.data(Qt::DisplayRole).toString();
    QString path  = index.data(FileListModel::FilePathRole).toString();
    QString size  = index.data(FileListModel::FileSizeRole).toString();

    QRect textRect = opt.rect.adjusted(textLeft, 5, -5, -5);

    bool selected = opt.state & QStyle::State_Selected;
    
    painter->setPen(selected
            ? opt.palette.highlightedText().color()
            : opt.palette.text().color());

    QFontMetrics fmName(opt.font);
    QString elidedName =
        fmName.elidedText(name, Qt::ElideRight, textRect.width());

    painter->drawText(textRect.adjusted(0, 0, 0, -20), elidedName);

    painter->setPen(selected 
            ? opt.palette.highlightedText().color()
            : QColor(120, 120, 120));

    QFontMetrics fmPath(opt.font);
    QString fullPath = path + " | " + size;

    QString elidedPath =
        fmPath.elidedText(fullPath, Qt::ElideMiddle, textRect.width());

    painter->drawText(textRect.adjusted(0, 20, 0, 0), elidedPath);

    painter->restore();
}

QSize FileItemDelegate::sizeHint(const QStyleOptionViewItem& option,
               const QModelIndex& index) const
{
    return QSize(200, 50);
}
