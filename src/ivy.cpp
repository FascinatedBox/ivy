#include <getopt.h>
#include <QApplication>
#include <QDir>

#include "ivyservice.h"

typedef enum {
    opt_minimized,
    opt_quit,
    opt_scroll_down,
    opt_scroll_left,
    opt_scroll_right,
    opt_scroll_up,
    opt_zoom_in,
    opt_zoom_out,
    opt_help = 'h',
} optlist_t;

struct option longopts[] = {
    { "quit", no_argument, NULL, opt_quit },
    { "help", no_argument, NULL, opt_help },
    { "minimized", no_argument, NULL, opt_minimized },
    { "scroll-down", no_argument, NULL, opt_scroll_down },
    { "scroll-left", no_argument, NULL, opt_scroll_left },
    { "scroll-right", no_argument, NULL, opt_scroll_right },
    { "scroll-up", no_argument, NULL, opt_scroll_up },
    { "zoom-in", no_argument, NULL, opt_zoom_in },
    { "zoom-out", no_argument, NULL, opt_zoom_out },
    { NULL, 0, NULL, 0 },
};

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
R"(Usage: ivy [options] [<actions>...] files...

Options:
    --minimized            Start the display minimized

Actions:
    -h, --help             Display this help and exit
    --quit                 Close the display and quit
    --scroll-<DIRECTION>   Scroll in a particular direction
                           (left, up, right, down)
    --zoom-<in|out>        Adjust the zoom on the image
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

    IvyService service = new IvyService(&app);

    service.registerService();

    if (service.isRegistered()) {
        app.setStyleSheet(loadFile(":/style.qss"));
        service.start();
    }

    bool minimized;
    int c, option_index;

    while ((c = getopt_long_only(argc, argv, "h",
                                 longopts, &option_index)) != -1) {
        switch (c) {
            case opt_help:
                showHelpAndExit();
                break;
            case opt_minimized:
                service.setMinimized(true);
                break;
            case opt_quit:
                service.quit();
                return EXIT_SUCCESS;
            case opt_scroll_down:
                service.scrollDown();
                break;
            case opt_scroll_left:
                service.scrollLeft();
                break;
            case opt_scroll_right:
                service.scrollRight();
                break;
            case opt_scroll_up:
                service.scrollUp();
                break;
            case opt_zoom_in:
                service.zoomIn();
                break;
            case opt_zoom_out:
                service.zoomOut();
                break;
        }
    }

    service.show();

    QString currentDirPath = QDir::currentPath();

    while (optind != argc) {
        QString s = argv[optind];
        QString path = s;

        if (QDir::isAbsolutePath(s) == false)
            path = currentDirPath + "/" + path;

        service.open(path);
        optind++;
    }

    if (service.isRegistered() == false)
        exit(EXIT_SUCCESS);

    return app.exec();
}
