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
    _scale = 1.0;

    QWidget *w = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;

    _picLabel = new QLabel;
    _picLabel->setScaledContents(true);
    _picLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    _scrollArea = new IvyScrollArea;
    _scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _scrollArea->setVisible(true);
    _scrollArea->setWidget(_picLabel);
    _scrollArea->setWidgetResizable(false);

    _historyListWidget = new QListWidget;
    _historyListWidget->setFixedWidth(70);
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
    _scale *= s;
    _picLabel->resize(_scale * _picLabel->pixmap()->size());

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
    scaleImage(1.25);
}

void IvyMainWindow::zoomOut()
{
    scaleImage(0.8);
}

void IvyMainWindow::resetZoom()
{
    _picLabel->adjustSize();
    _scale = 1.0;
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

void IvyMainWindow::onListRowChanged(int row)
{
    QPixmap pixmap = _pixmapStack[row];

    _scale = 1.0;
    _picLabel->setPixmap(pixmap);
    _picLabel->adjustSize();
    setWindowFilePath(_pathStack[row]);
    _zoomInAct->setEnabled(true);
    _zoomOutAct->setEnabled(true);
    _resetZoomAct->setEnabled(true);
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
