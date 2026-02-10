#ifndef FILELISTVIEW_H
#define FILELISTVIEW_H

#include <QListView>
#include <QVector>

class FileListView : public QListView
{
private:
    Q_OBJECT
public:
    FileListView(QWidget* parent = nullptr);
protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QVector<int> selectedRows() const;
};

#endif // FILELISTVIEW_H
