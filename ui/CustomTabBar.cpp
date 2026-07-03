#include "CustomTabBar.h"

#include <QMouseEvent>

void CustomTabBar::mousePressEvent(QMouseEvent *event)
{
    int index = tabAt(event->pos());
    if(index <= -1)
        return;

    if(event->button() == Qt::LeftButton)
    {
        emit switchTab(index);
    }
    else if(event->button() == Qt::RightButton)
    {
        emit openMenuTab(index);
    }
}
