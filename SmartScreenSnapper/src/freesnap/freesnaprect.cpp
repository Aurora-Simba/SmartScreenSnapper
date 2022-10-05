#include "freesnaprect.h"
#include <QMouseEvent>

FreeSnapRect::FreeSnapRect(QWidget* parent, Qt::WindowFlags f) : QFrame(parent)
{
    setMouseTracking(true);
}

void FreeSnapRect::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMove(event->pos());
    QWidget::mouseMoveEvent(event);
}
