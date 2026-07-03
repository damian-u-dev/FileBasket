#ifndef CUSTOMTABBAR_H
#define CUSTOMTABBAR_H

#include <QTabBar>

class QMouseEvent;

class CustomTabBar : public QTabBar
{
    Q_OBJECT
public:
    using QTabBar::QTabBar;

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void switchTab(int index);
    void openMenuTab(int index);
};

#endif // CUSTOMTABBAR_H
