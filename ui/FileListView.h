#ifndef FILELISTVIEW_H
#define FILELISTVIEW_H

#include <QListView>

class FileListView : public QListView
{
private:
    Q_OBJECT
public:
    FileListView(QWidget* parent = nullptr);
};

#endif // FILELISTVIEW_H
