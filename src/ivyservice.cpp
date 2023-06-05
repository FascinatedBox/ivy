#include <QDBusConnection>

#include <ivyadaptor.h>

#include "ivyservice.h"

#define SERVICE_NAME   "box.ivy.IvyServer"
#define PATH_NAME      "/ivy"
#define INTERFACE_NAME "box.ivy.Ivy"

bool IvyService::registerService()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    _registered = dbus.registerService(SERVICE_NAME);
    _minimized = false;
    _mw = nullptr;

    if (_registered) {
        /* This is necessary because IvyAdaptor reads messages off of dbus and
           sends them over to IvyService. */
        new IvyAdaptor(this);
        dbus.registerObject(PATH_NAME, this);
        return true;
    }
    else {
        _iface = new QDBusInterface(SERVICE_NAME, PATH_NAME, INTERFACE_NAME, dbus);
        return _iface->isValid();
    }
}

void IvyService::start()
{
    if (_registered == false)
        return;

    _mw = new IvyMainWindow();
}

void IvyService::show()
{
    if (_registered == false)
        return;

    if (_minimized)
        _mw->showMinimized();
    else
        _mw->show();
}

bool IvyService::open(QString path)
{
    if (_registered)
        return _mw->busOpen(path);
    else {
        _iface->call("open", path);
        return true;
    }
}

void IvyService::close()
{
    if (_registered)
        _mw->close();
    else
        _iface->call("close");
}

void IvyService::scrollDown()
{
    if (_registered)
        _mw->busScrollDown();
    else
        _iface->call("scrollDown");
}

void IvyService::scrollLeft()
{
    if (_registered)
        _mw->busScrollLeft();
    else
        _iface->call("scrollLeft");
}

void IvyService::scrollRight()
{
    if (_registered)
        _mw->busScrollRight();
    else
        _iface->call("scrollRight");
}

void IvyService::scrollUp()
{
    if (_registered)
        _mw->busScrollUp();
    else
        _iface->call("scrollUp");
}

void IvyService::zoomIn()
{
    if (_registered)
        _mw->zoomIn();
    else
        _iface->call("zoomIn");
}

void IvyService::zoomOut()
{
    if (_registered)
        _mw->zoomOut();
    else
        _iface->call("zoomDown");
}
