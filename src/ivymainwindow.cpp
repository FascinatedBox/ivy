#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QScreen>
#include <QScrollBar>

#include "ivymainwindow.h"

#define SCALE_FACTOR 0.905
#define THUMBNAIL_SIZE 100
#define THUMBNAIL_QSIZE QSize(100, 100)

IvyMainWindow::IvyMainWindow()
{
    _emptyPixmap = QPixmap();
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
    _historyListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _historyListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(_historyListWidget);
    layout->addWidget(_scrollArea);
    w->setLayout(layout);

    QGuiApplication::setApplicationDisplayName(tr("Ivy"));

    connect(_scrollArea, &IvyScrollArea::zoomedIn,
            this, &IvyMainWindow::zoomIn);
    connect(_scrollArea, &IvyScrollArea::zoomedOut,
            this, &IvyMainWindow::zoomOut);
    connect(_historyListWidget, &QListWidget::currentItemChanged,
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

    _fitToWindowAct = viewMenu->addAction(tr("Fit To Window"), this, &IvyMainWindow::fitToWindow);
    _fitToWindowAct->setShortcut(tr("f"));
    _fitToWindowAct->setEnabled(false);

    viewMenu->addSeparator();

    _removeAct = viewMenu->addAction(tr("Remove From List"), this, &IvyMainWindow::removeFromList);
    _removeAct->setShortcut(QKeySequence::Delete);
    _removeAct->setEnabled(false);
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

void IvyMainWindow::fitToWindow()
{
    _scale = imageScaleForArea(_currentPixmap, _scrollArea->size());
    scaleImage(_scale);
}

void IvyMainWindow::removeFromList()
{
    if (_removeAct->isEnabled()) {
        int row = _historyListWidget->currentRow();

        // The item has to be deleted first or the item change slot will have
        // the stacks at a different length than the preview list.
        delete _historyListWidget->takeItem(row);
        _pixmapStack.removeAt(row);
        _pathStack.removeAt(row);
    }
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

void IvyMainWindow::onListRowChanged(QListWidgetItem *current,
        QListWidgetItem *prev)
{
    _zoomInAct->setEnabled(current != nullptr);
    _zoomOutAct->setEnabled(current != nullptr);
    _resetZoomAct->setEnabled(current != nullptr);
    _fitToWindowAct->setEnabled(current != nullptr);
    _removeAct->setEnabled(current != nullptr);

    QString path;

    if (current != nullptr) {
        int currentRow = _historyListWidget->row(current);

        _currentPixmap = &_pixmapStack[currentRow];
        _scale = imageScaleForArea(_currentPixmap, _scrollArea->size());
        scaleImage(_scale);
        path = _pathStack[currentRow];
    }
    else {
        _currentPixmap = &_emptyPixmap;
        path = "";
    }

    _picLabel->setPixmap(*_currentPixmap);
    setWindowFilePath(path);
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
