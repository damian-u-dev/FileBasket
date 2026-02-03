#include "window.h"
#include "./ui_window.h"

#include <QVector>
#include <QString>
#include <QStringList>

Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Window)
{
    ui->setupUi(this);

    connect(ui->buttonAdd, &QPushButton::clicked, this, &Window::onAddClicked);
    connect(ui->buttonCopy, &QPushButton::clicked, this, &Window::onCopyClicked);
    connect(ui->buttonMove, &QPushButton::clicked, this, &Window::onMoveClicked);
}

void Window::onAddClicked()
{

}

void Window::onCopyClicked()
{

}

void Window::onMoveClicked()
{

}

Window::~Window()
{
    qInfo() <<"---Shutting down...---\n\n";
    delete ui;
}
