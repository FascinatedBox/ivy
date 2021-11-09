#include <QCoreApplication>
#include <QDir>
#include <QtDBus>
#include <getopt.h>

#define SERVICE_NAME   "box.ivy.IvyServer"
#define PATH_NAME      "/ivy"
#define INTERFACE_NAME "box.ivy.Ivy"

typedef enum {
    opt_close,
    opt_help,
    opt_open,
    opt_scroll_down,
    opt_scroll_left,
    opt_scroll_right,
    opt_scroll_up,
    opt_zoom_in,
    opt_zoom_out,
} optlist_t;

struct option longopts[] = {
    { "close", no_argument, NULL, opt_close },
    { "help", no_argument, NULL, opt_help },
    { "open", required_argument, NULL, opt_open },
    { "scroll-down", no_argument, NULL, opt_scroll_down },
    { "scroll-left", no_argument, NULL, opt_scroll_left },
    { "scroll-right", no_argument, NULL, opt_scroll_right },
    { "scroll-up", no_argument, NULL, opt_scroll_up },
    { "zoom-in", no_argument, NULL, opt_zoom_in },
    { "zoom-out", no_argument, NULL, opt_zoom_out },
};

QDBusInterface *iface = nullptr;

void showHelpAndExit()
{
    puts(
R"(Usage: ivyclient [<actions>...]

Actions:
    -o | --open PATH:
        Load PATH in the server and switch to it.
        Images are *not* reloaded if the file changes.

    --close:               Close the server and quit.
    --scroll-<DIRECTION>:  Scroll in a particular direction
                           (left, up, right, down)
    --zoom-<in|out>:       Adjust the zoom on the image
    -h | --help:           Show help (this message) and exit
)"
    );
    exit(EXIT_SUCCESS);
}

void doOpen(QString path)
{
    QDir baseDir = QDir(QDir::currentPath());
    QString absPath = baseDir.absoluteFilePath(path);

    if (baseDir.exists(absPath) == false) {
        qCritical("ivyclient: Cannot open '%s'.", qUtf8Printable(absPath));
        return;
    }

    QDBusReply<bool> reply = iface->call("open", absPath);

    if (reply.value() == false)
        qCritical("ivyclient: Server cannot open '%s'.",
                qUtf8Printable(absPath));
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QDBusConnection dbus = QDBusConnection::sessionBus();

    /* Don't need the server running just to give out help. */
    if (argc > 1 &&
        (strcmp(argv[1], "-h") == 0 ||
         strcmp(argv[1], "--help") == 0))
        showHelpAndExit();

    if (!dbus.isConnected()) {
        qCritical("ivyclient: Cannot connect to the D-Bus session bus.");
        return EXIT_FAILURE;
    }

    iface = new QDBusInterface(SERVICE_NAME, PATH_NAME, INTERFACE_NAME, dbus);

    if (iface->isValid() == false) {
        qCritical("ivyclient: Cannot connect to ivyserver. Is it running?");
        return EXIT_FAILURE;
    }

    int c, option_index;

    while ((c = getopt_long_only(argc, argv, "ho:",
                                 longopts, &option_index)) != -1) {
        switch (c) {
            case opt_close:
                iface->call("close");
                return EXIT_SUCCESS;
            case 'h':
            case opt_help:
                showHelpAndExit();
                break;
            case 'o':
            case opt_open:
                doOpen(optarg);
                break;
            case opt_scroll_down:
                iface->call("scrollDown");
                break;
            case opt_scroll_left:
                iface->call("scrollLeft");
                break;
            case opt_scroll_right:
                iface->call("scrollRight");
                break;
            case opt_scroll_up:
                iface->call("scrollUp");
                break;
            case opt_zoom_in:
                iface->call("zoomIn");
                break;
            case opt_zoom_out:
                iface->call("zoomOut");
                break;
        }
    }

    if (optind != argc) {
        qCritical("ivyclient: Invalid argument '%s'.", argv[optind]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
