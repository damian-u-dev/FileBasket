#include "FileListView.h"
#include "../controller/FileBasketController.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QMimeData>

FileListView::FileListView(QWidget* parent)
    : QListView(parent)
{
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void FileListView::setController(FileBasketController* ctrl)
{
    controller = ctrl;
}

void FileListView::contextMenuEvent(QContextMenuEvent* event)
{
    QVector<int> rows = selectedRows();
    if(rows.isEmpty())
        return;

    QMenu menu(this);

    bool multiple = rows.size() > 1;

    QAction* removeAction = menu.addAction(
        multiple ? "Remove elements" : "Remove element");

    QAction* moveAction = menu.addAction(
        multiple ? "Move elements" : "Move element");

    QAction* chosen = menu.exec(event->globalPos());

    if(!chosen)
        return;

    if(chosen == removeAction)
    {
        controller->removeItems(rows);
    }
    else if(chosen == moveAction)
    {
        controller->moveItems(rows);
    }
}

QVector<int> FileListView::selectedRows() const
{
    QVector<int> rows;
    QModelIndexList selected = selectionModel()->selectedIndexes();

    for(const QModelIndex& index : std::as_const(selected))
        rows.push_back(index.row());

    return rows;
}

void FileListView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void FileListView::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void FileListView::dropEvent(QDropEvent* event)
{
    const QList<QUrl> urls = event->mimeData()->urls();

    QStringList paths;

    for(const QUrl& url : urls)
        paths.append(url.toLocalFile());

    emit filesDropped(paths);
}
