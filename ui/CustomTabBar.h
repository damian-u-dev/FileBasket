#ifndef CUSTOMTABBAR_H
#define CUSTOMTABBAR_H

#include <QTabBar>

class CustomTabBar : public QTabBar
{
    Q_OBJECT
public:
    using QTabBar::QTabBar;

protected:
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // CUSTOMTABBAR_H
