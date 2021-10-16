#ifndef IVYMAINWINDOW_H
# define IVYMAINWINDOW_H
# include <QAction>
# include <QListWidget>
# include <QGraphicsScene>
# include <QGraphicsPixmapItem>
# include <QMainWindow>

# include "ivygraphicsview.h"

class IvyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    IvyMainWindow();

    bool busOpen(QString path);
    void busScrollDown();
    void busScrollLeft();
    void busScrollRight();
    void busScrollUp();

public slots:
    void zoomIn();
    void zoomOut();

private slots:
    void resetZoom();
    void onListRowChanged(int);

private:
    void addThumbnailForPixmap(const QPixmap *);
    bool loadFile(const QString &);
    void scaleImageToView(const QPixmap *);
    void setPixmap(const QPixmap &, QString);
    void setupMenuBar();

    QListWidget *_historyListWidget;
    IvyGraphicsView _view;
    QGraphicsScene _scene;
    QGraphicsPixmapItem _item;
    QList<QPixmap> _pixmapStack;
    QList<QString> _pathStack;

    QAction *_resetZoomAct;
    QAction *_zoomInAct;
    QAction *_zoomOutAct;
};

#endif
