#include "ivygraphicsview.h"

void IvyGraphicsView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier) {
        int forward = e->angleDelta().y() > 0;

        if (forward)
            emit zoomedIn();
        else
            emit zoomedOut();
    }
    else
        QGraphicsView::wheelEvent(e);
}
