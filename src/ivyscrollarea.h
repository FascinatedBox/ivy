#ifndef IVYSCROLLAREA_H
# define IVYSCROLLAREA_H
# include <QScrollArea>
# include <QWheelEvent>

class IvyScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    IvyScrollArea() {};
    void wheelEvent(QWheelEvent *e);

signals:
    void zoomedIn();
    void zoomedOut();
};

#endif
