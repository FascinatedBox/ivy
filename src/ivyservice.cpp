#include "ivyadaptor.h"
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

#define SERVICE_FN(name, window_fn) \
void IvyService::name() \
{ \
    if (_registered) \
        _mw->window_fn(); \
    else \
        _iface->call(#name); \
}

SERVICE_FN(quit,        close)
SERVICE_FN(scrollDown,  busScrollDown)
SERVICE_FN(scrollLeft,  busScrollLeft)
SERVICE_FN(scrollRight, busScrollRight)
SERVICE_FN(scrollUp,    busScrollUp)
SERVICE_FN(zoomIn,      zoomIn)
SERVICE_FN(zoomOut,     zoomOut)
