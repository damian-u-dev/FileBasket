#ifndef WINDOW_H
#define WINDOW_H

#include "CustomTabBar.h"

#include <QMainWindow>
#include <QTabBar>

class AppModel;
class FileBasketController;
class FileListModel;

QT_BEGIN_NAMESPACE
namespace Ui {
class Window;
}
QT_END_NAMESPACE

class Window : public QMainWindow
{
private:
    Q_OBJECT
    AppModel& model;
    FileBasketController& controller;
    FileListModel* fileListModel = nullptr;
    CustomTabBar* tabBar;
    QString currentTheme;
    QStringList themes;

public:
    Window(AppModel& model, FileBasketController& ctrl, QWidget *parent = nullptr);
    ~Window();


private:
    void setupUi();
    void setupListView();
    void setupConnections();
    void setupAnimations();
    void setupEffects();
    void setupTabBar();
    void setupUpdater();
    void setupMenuBar();
    void setupTheme();
    void setTitle();

    void buildTabs(const QStringList& names);
    void createNewTab();
    void clearTabs();
    void renameTab(int index);
    void deleteTab(int index);
    void enterActivationKey();
    void selectTheme();
    void setTheme(const QString& nameTheme);

signals:
    void requestAddFiles(const QStringList& paths);
    void requestCopy(const QString& targetDir, const QVector<int>& selectedIndices);
    void requestMove(const QString& targetDir, const QVector<int>& selectedIndices);
    void requestActiveTabChanged(int index);

private slots:
    void onAddClicked();
    void onCopyClicked();
    void onMoveClicked();
    void updateStatusBar();
    void onClickTab(int index);
    void rebuildTabs();
    void onTabContextMenu(const QPoint& pos);

private:
    Ui::Window *ui;
};
#endif // WINDOW_H
