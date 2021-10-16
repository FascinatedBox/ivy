#include <QApplication>

#include "ivyservice.h"

QString loadFile(QString path)
{
    QFile f(path);
    f.open(QIODevice::ReadOnly);
    QString s = f.readAll();
    f.close();

    return s;
}

int main(int ac, char **av)
{
    QApplication app(ac, av);
    app.setApplicationName("ivy");

    IvyService i(&app);

    if (i.tryRegisterService() == false) {
        qCritical("ivyserver is already running. Stopping.");
        exit(EXIT_FAILURE);
    }

    app.setStyleSheet(loadFile(":/style.css"));
    i.start();

    return app.exec();
}
