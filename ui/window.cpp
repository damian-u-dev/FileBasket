#include "window.h"
#include "./ui_window.h"
#include "../model/AppModel.h"
#include "../model/FileListModel.h"
#include "../controller/FileBasketController.h"
#include "FileItemDelegate.h"
#include "CustomTabBar.h"
#include "../services/UpdateCheckerService.h"
#include "../services/LicenseService.h"

#include <QVector>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QInputDialog>
#include <QMessageBox>
#include <QDesktopServices>

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
    setupUpdater();
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
    updateStatusBar();
    setupMenuBar();
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
    connect(&model, &AppModel::activeTabChanged, fileListModel, &FileListModel::onCurrentTabChanged);
}

void Window::setupConnections()
{
    connect(ui->buttonAdd, &QPushButton::clicked, this, &Window::onAddClicked);
    connect(ui->buttonCopy, &QPushButton::clicked, this, &Window::onCopyClicked);
    connect(ui->buttonMove, &QPushButton::clicked, this, &Window::onMoveClicked);

    connect(&model, &AppModel::modelChanged, this, &Window::updateStatusBar);
    connect(&model, &AppModel::tabsChanged, this, &Window::rebuildTabs);

    connect(ui->listView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &Window::updateStatusBar);

    connect(ui->listView,
            &FileListView::filesDropped,
            &controller,
            &FileBasketController::handleDrop);
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
    if(model.isAppModelEmpty())
        return;

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
    QHBoxLayout* tabLayout = new QHBoxLayout();
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(5);

    tabBar = new CustomTabBar(this);

    QPushButton* addButton = new QPushButton("+", this);
    addButton->setFixedSize(43, 43);
    addButton->setCursor(Qt::PointingHandCursor);

    tabLayout->addWidget(tabBar);
    tabLayout->addWidget(addButton);

    ui->verticalLayout_3->insertLayout(0, tabLayout);

    connect(tabBar, &QTabBar::customContextMenuRequested, this, &Window::onTabContextMenu);
    connect(tabBar, &QTabBar::tabBarClicked, this, &Window::onClickTab);
    connect(addButton, &QPushButton::clicked, this, &Window::createNewTab);

    tabBar->setExpanding(false);
    tabBar->setUsesScrollButtons(true);
    tabBar->setElideMode(Qt::ElideRight);
    tabBar->setDocumentMode(true);
    tabBar->setTabsClosable(false);
    tabBar->setMovable(false);
    tabBar->setContextMenuPolicy(Qt::CustomContextMenu);

    buildTabs(model.getTabNames());
    tabBar->setCurrentIndex(model.getIndexActiveTab());
    setTitle();
}

void Window::onClickTab(int index)
{
    model.setActiveTab(index);
    setTitle();
}

void Window::buildTabs(const QStringList& names)
{
    for(const QString& name : names)
    {
        tabBar->addTab(name);
    }
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
    QSignalBlocker blocker(tabBar);

    clearTabs();
    buildTabs(model.getTabNames());

    tabBar->setCurrentIndex(model.getIndexActiveTab());
}

void Window::clearTabs()
{
    while (tabBar->count() > 0)
    {
        tabBar->removeTab(0);
    }
}

void Window::setTitle()
{
    QString nameProgram = "FileBasket";
    if(LicenseService::isPro())
        nameProgram = "File Basket Pro";

    QString tab = model.getNameActiveTab();
    setWindowTitle(QString("%1 - %2").arg(tab).arg(nameProgram));
}

void Window::onTabContextMenu(const QPoint& pos)
{
    int index = tabBar->tabAt(pos);

    if(index < 0)
        return;

    QMenu menu(this);

    QAction* renameAction = menu.addAction("Rename Tab");
    renameAction->setIcon(QIcon(":/UI/resources/rename_tab.ico"));

    QAction* deleteAction = menu.addAction("Delete Tab");
    deleteAction->setIcon(QIcon(":/UI/resources/delete_tab.ico"));

    QAction* selected =
        menu.exec(tabBar->mapToGlobal(pos));

    if(selected == renameAction)
    {
        renameTab(index);
    }
    if(selected == deleteAction)
    {
        deleteTab(index);
    }
}

void Window::renameTab(int index)
{
    QString oldName = model.getTabName(index);

    bool ok;

    QString newName = QInputDialog::getText(
        this,
        "Rename tab",
        "New tab name:",
        QLineEdit::Normal,
        oldName,
        &ok
        );

    if(!ok || newName.trimmed().isEmpty())
        return;

    controller.renameTab(index, newName);
}

void Window::deleteTab(int index)
{
    QString name = model.getTabName(index);

    QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            "Delete tab",
            QString("Delete tab \"%1\"?").arg(name)
            );

    if(answer == QMessageBox::Yes)
    {
        controller.deleteTab(index);
    }
}

void Window::setupUpdater()
{
    UpdateCheckerService* updater = new UpdateCheckerService(this);
    connect(updater, &UpdateCheckerService::updateAvailable, this,
            [=](QString version, QString url)
        {
            QMessageBox::information(this, "Update available",
                QString("New version %1 is available!\n\nDownload?").arg(version));
            QDesktopServices::openUrl(QUrl(url));
        });
    updater->checkForUpdates();
}

void Window::setupMenuBar()
{
    ui->menuSettings->setIcon(QIcon(":/UI/resources/settings.ico"));
    connect(ui->actionActivate, &QAction::triggered, this, &Window::enterActivationKey);
}

void Window::enterActivationKey()
{
    if(LicenseService::isPro())
    {
        QMessageBox::information(this, "Activation", "You're already activated!");
        return;
    }

    bool ok;

    const QString key = QInputDialog::getText(this,
                          "Activation",
                          "Please enter the key that was sent to your email.\nExample: Alex-XXXXXXXXXXX",
                          QLineEdit::Normal,
                          QString(),
                          &ok);

    if(!ok || key.isEmpty())
        return;

    if(LicenseService::activate(key))
    {
        QMessageBox::information(this, "Activation", "Congratulations. You're activated!");
    }
    else
    {
        QMessageBox::information(this, "Activation", "You're license key is not correct!");
    }
}
