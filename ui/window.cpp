#include "window.h"
#include "./ui_window.h"
#include "../model/AppModel.h"
#include "../model/FileListModel.h"

#include <QVector>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QAbstractItemView>

Window::Window(AppModel& model, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Window)
    , model(model)
{
    ui->setupUi(this);

    FileListModel* fileListModel = new FileListModel(model, this);
    ui->listView->setModel(fileListModel);
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->listView->setAcceptDrops(true);
    ui->listView->setDragDropMode(QAbstractItemView::DropOnly);
    ui->listView->setDefaultDropAction(Qt::CopyAction);

    QObject::connect(&model, &AppModel::filesAdded, fileListModel, &FileListModel::onFilesAdded);

    connect(ui->buttonAdd, &QPushButton::clicked, this, &Window::onAddClicked);
    connect(ui->buttonCopy, &QPushButton::clicked, this, &Window::onCopyClicked);
    connect(ui->buttonMove, &QPushButton::clicked, this, &Window::onMoveClicked);
}

void Window::onAddClicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Add files");
    if(!files.empty())
        emit requestAddFiles(files);
}

void Window::onCopyClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Copy to");
    if(dir.isEmpty())
        return;

    QVector<int> selected; //TODO: Get indices of selected files;
    emit requestCopy(dir, selected);
}

void Window::onMoveClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Move to");
    if(dir.isEmpty())
        return;

    QVector<int> selected; //TODO: Get indices of selected files;
    emit requestMove(dir, selected);
}

Window::~Window()
{
    qInfo() <<"---Shutting down...---\n\n";
    delete ui;
}
