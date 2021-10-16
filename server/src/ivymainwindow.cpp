#include <QLabel>
#include <QFileDialog>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QImageReader>
#include <QImageWriter>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QScrollBar>
#include <QStandardPaths>

#include "ivymainwindow.h"

IvyMainWindow::IvyMainWindow()
{
    _view.setScene(&_scene);
    _scene.addItem(&_item);
    _view.setResizeAnchor(QGraphicsView::AnchorViewCenter);

    QWidget *w = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;

    _historyListWidget = new QListWidget;
    _historyListWidget->setFixedWidth(70);
    _historyListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(_historyListWidget);
    layout->addWidget(&_view);
    w->setLayout(layout);

    QGuiApplication::setApplicationDisplayName(tr("Ivy"));

    connect(&_view, &IvyGraphicsView::zoomedIn,
            this, &IvyMainWindow::zoomIn);
    connect(&_view, &IvyGraphicsView::zoomedOut,
            this, &IvyMainWindow::zoomOut);
    connect(_historyListWidget, &QListWidget::currentRowChanged,
            this, &IvyMainWindow::onListRowChanged);

    setupMenuBar();
    setCentralWidget(w);
    resize(QGuiApplication::primaryScreen()->availableSize() * .3);
    show();
}

void IvyMainWindow::setupMenuBar()
{
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    _zoomInAct = viewMenu->addAction(tr("Zoom In"), this, &IvyMainWindow::zoomIn);
    _zoomInAct->setShortcut(QKeySequence::ZoomIn);
    _zoomInAct->setEnabled(false);

    _zoomOutAct = viewMenu->addAction(tr("Zoom Out"), this, &IvyMainWindow::zoomOut);
    _zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    _zoomOutAct->setEnabled(false);

    _resetZoomAct = viewMenu->addAction(tr("Reset Zoom"), this, &IvyMainWindow::resetZoom);
    _resetZoomAct->setShortcut(tr("r"));
    _resetZoomAct->setEnabled(false);
}

void IvyMainWindow::zoomIn()
{
    _view.scale(1.1, 1.1);
}

void IvyMainWindow::zoomOut()
{
    _view.scale(.9, .9);
}

void IvyMainWindow::resetZoom()
{
    _view.resetMatrix();
}

void IvyMainWindow::addThumbnailForPixmap(const QPixmap *pixmap)
{
    QPixmap thumbnail = pixmap->scaled(64, 64, Qt::KeepAspectRatio);
    QLabel *l = new QLabel();

    l->setAlignment(Qt::AlignCenter);
    l->setPixmap(thumbnail);

    QListWidgetItem *newItem = new QListWidgetItem();

    newItem->setSizeHint(QSize(70,70));

    _historyListWidget->addItem(newItem);
    _historyListWidget->setItemWidget(newItem, l);
    _historyListWidget->setCurrentItem(newItem);
}

void IvyMainWindow::scaleImageToView(const QPixmap *pixmap)
{
    /* Find out how to show as much of the image as possible without showing
       scrollbars. */
    QSize viewSize = _view.size();
    QSize pixmapSize = pixmap->size();
    double viewHeight = _view.height();
    double viewWidth = _view.width();
    double pixmapHeight = pixmapSize.height();
    double pixmapWidth = pixmapSize.width();
    double scale = 1.0;

    while (pixmapHeight > viewHeight ||
           pixmapWidth > viewWidth) {
        pixmapWidth *= .9;
        pixmapHeight *= .9;
        scale *= .9;
    }

    if (scale != 1.0)
        _view.scale(scale, scale);
}

void IvyMainWindow::setPixmap(const QPixmap &pixmap, QString path)
{
    _pixmapStack.append(pixmap);
    _pathStack.append(path);
    addThumbnailForPixmap(&pixmap);
}

void IvyMainWindow::onListRowChanged(int row)
{
    QPixmap pixmap = _pixmapStack[row];

    _item.setPixmap(pixmap);
    _view.resetMatrix();

    QSize s = pixmap.size();

    /* Fix scrollbars in case old image was larger than this one. */
    _view.setSceneRect(0, 0, s.width(), s.height());
    scaleImageToView(&pixmap);

    setWindowFilePath(_pathStack[row]);

    _zoomInAct->setEnabled(true);
    _zoomOutAct->setEnabled(true);
    _resetZoomAct->setEnabled(true);
}

bool IvyMainWindow::busOpen(QString path)
{
    QScrollBar *vbar = _view.verticalScrollBar();

    QPixmap pixmap;
    bool ok = pixmap.load(path);

    if (ok)
        setPixmap(pixmap, path);

    return ok;
}

void IvyMainWindow::busScrollDown()
{
    QScrollBar *vbar = _view.verticalScrollBar();
    int step = vbar->singleStep() * 3;

    vbar->setValue(vbar->value() + step);
}

void IvyMainWindow::busScrollLeft()
{
    QScrollBar *hbar = _view.horizontalScrollBar();
    int step = hbar->singleStep() * 3;

    hbar->setValue(hbar->value() - step);
}

void IvyMainWindow::busScrollRight()
{
    QScrollBar *hbar = _view.horizontalScrollBar();
    int step = hbar->singleStep() * 3;

    hbar->setValue(hbar->value() + step);
}

void IvyMainWindow::busScrollUp()
{
    QScrollBar *vbar = _view.verticalScrollBar();
    int step = vbar->singleStep() * 3;

    vbar->setValue(vbar->value() - step);
}
