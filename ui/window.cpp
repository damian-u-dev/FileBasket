#include "window.h"
#include "./ui_window.h"
#include "../model/AppModel.h"
#include "../model/FileListModel.h"
#include "../controller/FileBasketController.h"

#include <QVector>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

Window::Window(AppModel& model, FileBasketController& ctrl, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Window)
    , model(model)
    , controller(ctrl)
{
    ui->setupUi(this);
    setupUi();
    setupAnimations();
    setupEffects();
    setupConnections();
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

void Window::setupUi()
{
    setupListView();
}

void Window::setupListView()
{
    FileListModel* fileListModel = new FileListModel(model, this);
    ui->listView->setModel(fileListModel);
    ui->listView->setController(&controller);
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->listView->setAcceptDrops(true);
    ui->listView->setDragDropMode(QAbstractItemView::DropOnly);
    ui->listView->setDefaultDropAction(Qt::CopyAction);

    ui->listView->setIconSize(QSize(25, 25));
    ui->listView->setUniformItemSizes(true);

    QObject::connect(&model, &AppModel::filesAdded, fileListModel, &FileListModel::onFilesAdded);
    QObject::connect(&model, &AppModel::filesRemoved, fileListModel, &FileListModel::onFilesRemoved);
}

void Window::setupConnections()
{
    connect(ui->buttonAdd, &QPushButton::clicked, this, &Window::onAddClicked);
    connect(ui->buttonCopy, &QPushButton::clicked, this, &Window::onCopyClicked);
    connect(ui->buttonMove, &QPushButton::clicked, this, &Window::onMoveClicked);
}

void Window::setupAnimations()
{
    QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(300);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
}

void Window::setupEffects()
{
    this->setAttribute(Qt::WA_TranslucentBackground);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 150));
    ui->centralwidget->setGraphicsEffect(shadow);
}
