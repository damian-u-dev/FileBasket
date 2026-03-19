#ifndef FILELISTVIEW_H
#define FILELISTVIEW_H

#include <QListView>
#include <QVector>

class FileBasketController;

class FileListView : public QListView
{
private:
    Q_OBJECT
public:
    FileListView(QWidget* parent = nullptr);
    void setController(FileBasketController* ctrl);
protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    FileBasketController* controller = nullptr;
    QVector<int> selectedRows() const;

signals:
    void filesDropped(const QStringList& paths);
};

#endif // FILELISTVIEW_H
