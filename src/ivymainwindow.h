#ifndef IVYMAINWINDOW_H
# define IVYMAINWINDOW_H
# include <QAction>
# include <QLabel>
# include <QListWidget>
# include <QGraphicsScene>
# include <QGraphicsPixmapItem>
# include <QMainWindow>

# include "ivyscrollarea.h"

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
    void setupMenuBar();
    void scaleImage(double);
    void adjustScrollBar(QScrollBar *);

    QLabel *_picLabel;
    QListWidget *_historyListWidget;
    IvyScrollArea *_scrollArea;

    QAction *_resetZoomAct;
    QAction *_zoomInAct;
    QAction *_zoomOutAct;

    QList<QPixmap> _pixmapStack;
    QList<QString> _pathStack;

    double _scale;
};

#endif
