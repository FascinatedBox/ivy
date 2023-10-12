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

#define SCALE_FACTOR 0.905
#define THUMBNAIL_SIZE 100
#define THUMBNAIL_QSIZE QSize(100, 100)

IvyMainWindow::IvyMainWindow()
{
    _scale = 1.0;

    QWidget *w = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;

    _currentPixmap = NULL;

    _picLabel = new QLabel;
    _picLabel->setScaledContents(true);
    _picLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    _scrollArea = new IvyScrollArea;
    _scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _scrollArea->setVisible(true);
    _scrollArea->setWidget(_picLabel);
    _scrollArea->setWidgetResizable(false);

    _historyListWidget = new QListWidget;
    _historyListWidget->setFixedWidth(THUMBNAIL_SIZE);
    _historyListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(_historyListWidget);
    layout->addWidget(_scrollArea);
    w->setLayout(layout);

    QGuiApplication::setApplicationDisplayName(tr("Ivy"));

    connect(_scrollArea, &IvyScrollArea::zoomedIn,
            this, &IvyMainWindow::zoomIn);
    connect(_scrollArea, &IvyScrollArea::zoomedOut,
            this, &IvyMainWindow::zoomOut);
    connect(_historyListWidget, &QListWidget::currentRowChanged,
            this, &IvyMainWindow::onListRowChanged);

    setupMenuBar();
    setCentralWidget(w);
    resize(QGuiApplication::primaryScreen()->availableSize() * .3);
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

void IvyMainWindow::scaleImage(double s)
{
    if (_pixmapStack.size() == 0)
        return;

    _scale = s;
    _picLabel->resize(_scale * _currentPixmap->size());

    adjustScrollBar(_scrollArea->horizontalScrollBar());
    adjustScrollBar(_scrollArea->verticalScrollBar());

    _zoomInAct->setEnabled(_scale < 3.0);
    _zoomOutAct->setEnabled(_scale > 0.333);
}

void IvyMainWindow::adjustScrollBar(QScrollBar *scrollBar)
{
    scrollBar->setValue(int(_scale * scrollBar->value()
                          + ((_scale - 1) * scrollBar->pageStep()/2)));
}

void IvyMainWindow::zoomIn()
{
    /* The if check is for mouse wheel events that are forwarded here. */
    if (_zoomInAct->isEnabled())
        scaleImage(_scale / SCALE_FACTOR);
}

void IvyMainWindow::zoomOut()
{
    if (_zoomOutAct->isEnabled())
        scaleImage(_scale * SCALE_FACTOR);
}

void IvyMainWindow::resetZoom()
{
    scaleImage(1.0);
}

double IvyMainWindow::imageScaleForArea(const QPixmap *pixmap, QSize area)
{
    int areaW = area.width();
    int areaH = area.height();
    int pixW = pixmap->size().width();
    int pixH = pixmap->size().height();
    int origW = pixW;
    int origH = pixH;
    double scale = 1.0;

    while (areaH < pixH || areaW < pixW) {
        scale *= SCALE_FACTOR;
        pixW = origW * scale;
        pixH = origH * scale;
    }

    return scale;
}

void IvyMainWindow::addThumbnailForPixmap(const QPixmap *pixmap)
{
    QLabel *l = new QLabel();
    double thumbScale = imageScaleForArea(pixmap, THUMBNAIL_QSIZE);
    int thumbWidth = pixmap->size().width() * thumbScale;
    int thumbHeight = pixmap->size().height() * thumbScale;

    QTransform transform;
    transform.scale(thumbScale, thumbScale);

    /* Using a smooth transformation produces a nicer image than simply asking
       for a scaled image. */
    QPixmap thumbnail = pixmap->transformed(transform,
            Qt::SmoothTransformation);

    l->setAlignment(Qt::AlignCenter);
    l->setPixmap(thumbnail);

    QListWidgetItem *newItem = new QListWidgetItem();

    newItem->setSizeHint(THUMBNAIL_QSIZE);

    _historyListWidget->addItem(newItem);
    _historyListWidget->setItemWidget(newItem, l);
    _historyListWidget->setCurrentItem(newItem);
}

void IvyMainWindow::onListRowChanged(int row)
{
    _currentPixmap = &_pixmapStack[row];
    _scale = imageScaleForArea(_currentPixmap, _scrollArea->size());
    _picLabel->setPixmap(*_currentPixmap);
    _zoomInAct->setEnabled(true);
    _zoomOutAct->setEnabled(true);
    _resetZoomAct->setEnabled(true);
    scaleImage(_scale);
    setWindowFilePath(_pathStack[row]);
}

bool IvyMainWindow::busOpen(QString path)
{
    QScrollBar *vbar = _scrollArea->verticalScrollBar();

    QPixmap pixmap;
    bool ok = pixmap.load(path);

    if (ok) {
        _pixmapStack.append(pixmap);
        _pathStack.append(path);
        addThumbnailForPixmap(&pixmap);
    }

    return ok;
}

void IvyMainWindow::busScrollDown()
{
    QScrollBar *vbar = _scrollArea->verticalScrollBar();
    int step = vbar->singleStep() * 3;

    vbar->setValue(vbar->value() + step);
}

void IvyMainWindow::busScrollLeft()
{
    QScrollBar *hbar = _scrollArea->horizontalScrollBar();
    int step = hbar->singleStep() * 3;

    hbar->setValue(hbar->value() - step);
}

void IvyMainWindow::busScrollRight()
{
    QScrollBar *hbar = _scrollArea->horizontalScrollBar();
    int step = hbar->singleStep() * 3;

    hbar->setValue(hbar->value() + step);
}

void IvyMainWindow::busScrollUp()
{
    QScrollBar *vbar = _scrollArea->verticalScrollBar();
    int step = vbar->singleStep() * 3;

    vbar->setValue(vbar->value() - step);
}
