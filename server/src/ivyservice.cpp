#include <ivyadaptor.h>

#include "ivyservice.h"

#define SERVICE_NAME   "box.ivy.IvyServer"
#define PATH_NAME      "/ivy"

bool IvyService::tryRegisterService()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    return dbus.registerService(SERVICE_NAME);
}

void IvyService::start()
{
    /* This is necessary because IvyAdaptor reads messages off of dbus and sends
       them over to IvyService. */
    new IvyAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject(PATH_NAME, this);

    mw = new IvyMainWindow();
    mw->show();
}

bool IvyService::open(QString path)
{
    return mw->busOpen(path);
}

void IvyService::close()
{
    mw->close();
}

void IvyService::scrollDown()
{
    mw->busScrollDown();
}

void IvyService::scrollLeft()
{
    mw->busScrollLeft();
}

void IvyService::scrollRight()
{
    mw->busScrollRight();
}

void IvyService::scrollUp()
{
    mw->busScrollUp();
}

void IvyService::zoomIn()
{
    mw->zoomIn();
}

void IvyService::zoomOut()
{
    mw->zoomOut();
}
