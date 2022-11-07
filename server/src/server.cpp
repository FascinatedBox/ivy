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

void showHelpAndExit()
{
    puts(
R"(Usage: ivyserver [arguments]

Arguments:
    --minimized:           Start with the main window minimized
                           (Ignored if the server is running)
    -h | --help:           Show help (this message) and exit
)"
    );
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("ivy");

    if (argc > 1 &&
        (strcmp(argv[1], "-h") == 0 ||
         strcmp(argv[1], "--help") == 0))
        showHelpAndExit();

    IvyService i(&app);
    bool minimized = false;

    if (i.tryRegisterService() == false) {
        qCritical("ivyserver is already running. Stopping.");
        exit(EXIT_FAILURE);
    }

    if (argc > 1 &&
        strcmp(argv[1], "--minimized") == 0)
        minimized = true;

    app.setStyleSheet(loadFile(":/server.qss"));
    i.start(minimized);

    return app.exec();
}
