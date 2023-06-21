#include "clickhandle.h"

ClickHandle::ClickHandle(QObject *parent)
    : QObject(parent)
{
    // Constructor implementation
}

bool ClickHandle::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            emit clicked(mouseEvent->x(), mouseEvent->y());
        }
    }

    // Call the base eventFilter for default processing
    return QObject::eventFilter(obj, event);
}
