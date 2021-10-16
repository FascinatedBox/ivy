#ifndef IVYGRAPHICSVIEW_H
# define IVYGRAPHICSVIEW_H
# include <QGraphicsView>
# include <QWheelEvent>

class IvyGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    IvyGraphicsView() {};
    void wheelEvent(QWheelEvent *e);

signals:
    void zoomedIn();
    void zoomedOut();
};

#endif
