#include "FileListView.h"

FileListView::FileListView(QWidget* parent)
    : QListView(parent)
{
    setAcceptDrops(true);
}
