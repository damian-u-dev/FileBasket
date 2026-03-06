#include "window.h"
#include "./ui_window.h"
#include "../model/AppModel.h"
#include "../model/FileListModel.h"
#include "../controller/FileBasketController.h"
#include "FileItemDelegate.h"

#include <QVector>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QInputDialog>

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

    QVector<int> selected;
    const QModelIndexList& rows = ui->listView->selectionModel()->selectedRows();

    for(const QModelIndex& index : rows)
    {
        selected.append(index.row());
    }

    if(selected.isEmpty())
        return;

    emit requestCopy(dir, selected);
}

void Window::onMoveClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Move to");
    if(dir.isEmpty())
        return;

    QVector<int> selected;
    const QModelIndexList& rows = ui->listView->selectionModel()->selectedRows();

    for(const QModelIndex& index : rows)
    {
        selected.append(index.row());
    }

    if(selected.isEmpty())
        return;

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
    setupTabBar();
}

void Window::setupListView()
{
    fileListModel = new FileListModel(model, this);
    ui->listView->setModel(fileListModel);

    FileItemDelegate* fileItemDelegate = new FileItemDelegate(this);
    ui->listView->setItemDelegate(fileItemDelegate);

    ui->listView->setController(&controller);
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->listView->setAcceptDrops(true);
    ui->listView->setDragDropMode(QAbstractItemView::DropOnly);
    ui->listView->setDefaultDropAction(Qt::CopyAction);

    ui->listView->setIconSize(QSize(25, 25));
    ui->listView->setUniformItemSizes(true);

    connect(&model, &AppModel::filesAdded,   fileListModel, &FileListModel::onFilesAdded);
    connect(&model, &AppModel::filesRemoved, fileListModel, &FileListModel::onFilesRemoved);
    connect(&model, &AppModel::filesUpdated, fileListModel, &FileListModel::onFilesUpdated);
    connect(&model, &AppModel::currentTabChanged, fileListModel, &FileListModel::onCurrentTabChanged);
}

void Window::setupConnections()
{
    connect(ui->buttonAdd, &QPushButton::clicked, this, &Window::onAddClicked);
    connect(ui->buttonCopy, &QPushButton::clicked, this, &Window::onCopyClicked);
    connect(ui->buttonMove, &QPushButton::clicked, this, &Window::onMoveClicked);

    connect(&model, &AppModel::modelChanged, this, &Window::updateStatusBar);
    connect(tabBar, &QTabBar::currentChanged, this, &Window::onClickTab);
    connect(&model, &AppModel::tabsChanged, this, &Window::rebuildTabs);

    connect(ui->listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &Window::updateStatusBar);
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

void Window::updateStatusBar()
{
    const auto& files = model.activeTab().files;

    int totalCount = files.size();
    qint64 totalSize = 0;

    for(const auto& file : files)
        totalSize += file.size;

    QModelIndexList selected =
        ui->listView->selectionModel()->selectedRows();

    if(!selected.isEmpty())
    {
        qint64 selectedSize = 0;

        for(const QModelIndex& index : std::as_const(selected))
        {
            int row = index.row();
            if(row >= 0 && row < files.size())
                selectedSize += files[row].size;
        }

        statusBar()->showMessage(
            QString("Selected: %1 | Size: %2")
                .arg(selected.size())
                .arg(QLocale().formattedDataSize(selectedSize)));
    }
    else
    {
        statusBar()->showMessage(
            QString("Total elements: %1 | Size: %2")
            .arg(totalCount)
            .arg(QLocale().formattedDataSize(totalSize)));
    }
}

void Window::setupTabBar()
{
    tabBar = new QTabBar(this);
    tabBar->setExpanding(true);
    tabBar->setTabsClosable(false);
    tabBar->setMovable(false);
    ui->verticalLayout_3->insertWidget(0, tabBar);

    buildTabs(model.tabNames());
}

void Window::onClickTab(int index)
{
    if(index == tabBar->count() - 1)
    {
        QSignalBlocker blocker(tabBar);

        tabBar->setCurrentIndex(model.getCurrentTabIndex());
        createNewTab();
        return;
    }
    model.setCurrentTab(index);
}

void Window::buildTabs(const QStringList& names)
{
    for(const QString& name : names)
    {
        tabBar->addTab(name);
    }
    tabBar->addTab("+");
}

void Window::createNewTab()
{
    bool ok;

    QString name = QInputDialog::getText(
        this,
        "Create tab",
        "Tab name:",
        QLineEdit::Normal,
        "",
        &ok);

    if(!ok || name.trimmed().isEmpty())
        return;

    controller.createTab(name);
}

void Window::rebuildTabs()
{
    clearTabs();
    buildTabs(model.tabNames());
}

void Window::clearTabs()
{
    while (tabBar->count() > 0)
    {
        tabBar->removeTab(0);
    }
}
