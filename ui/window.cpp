#include "window.h"
#include "./ui_window.h"
#include "../model/AppModel.h"

#include <QVector>
#include <QString>
#include <QStringList>
#include <QFileDialog>

Window::Window(AppModel& model, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Window)
    , model(model)
{
    ui->setupUi(this);

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
