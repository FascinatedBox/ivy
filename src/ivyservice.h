#ifndef IVYSERVICE_H
# define IVYSERVICE_H
# include <QDBusInterface>
# include <QObject>
# include "ivymainwindow.h"
# define INTERFACE_NAME "box.ivy.Ivy"

class IvyService : public QObject
{
    Q_OBJECT;
    Q_CLASSINFO("D-Bus Interface", INTERFACE_NAME);

public:
    IvyService(QObject *parent) {};
    bool isRegistered() { return _registered; }
    bool registerService();
    void start();
    void setMinimized(bool b) { _minimized = true; }
    void show();

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
    IvyMainWindow *_mw;
    QDBusInterface *_iface;
    bool _registered;
    bool _minimized;
};

#endif
