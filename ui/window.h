#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

class AppModel;

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
public:
    Window(AppModel& model, QWidget *parent = nullptr);
    ~Window();

signals:
    void requestAddFiles(const QStringList& paths);
    void requestCopy(const QString& targetDir, const QVector<int>& selectedIndices);
    void requestMove(const QString& targetDir, const QVector<int>& selectedIndices);
    void requestTabChanged(int index);

private slots:
    void onAddClicked();
    void onCopyClicked();
    void onMoveClicked();

private:
    Ui::Window *ui;
};
#endif // WINDOW_H
