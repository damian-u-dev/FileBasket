#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Window;
}
QT_END_NAMESPACE

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
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
