#ifndef IVYSERVICE_H
# define IVYSERVICE_H
# include <QObject>
# include "ivymainwindow.h"
# define INTERFACE_NAME "box.ivy.Ivy"

class IvyService : public QObject
{
    Q_OBJECT;
    Q_CLASSINFO("D-Bus Interface", INTERFACE_NAME);

public:
    IvyService(QObject *parent) {};
    bool tryRegisterService();
    void start();

public slots:
    bool open(QString path);
    void close();
    void scrollDown();
    void scrollLeft();
    void scrollRight();
    void scrollUp();
    void zoomIn();
    void zoomOut();

private:
    IvyMainWindow *mw;
};

#endif
