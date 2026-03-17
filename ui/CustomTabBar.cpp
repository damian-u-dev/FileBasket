#include "CustomTabBar.h"

#include <QMouseEvent>

void CustomTabBar::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        event->accept();
        return;
    }

    QTabBar::mousePressEvent(event);
}
