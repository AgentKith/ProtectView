#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "app/appcontroller.h"
#include "app/kiosk.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("ProtectView");
    app.setOrganizationName("ProtectView");

    QCommandLineParser parser;
    parser.setApplicationDescription("UniFi UNVR Camera Monitor");
    parser.addHelpOption();

    QCommandLineOption kioskOption("kiosk", "Run in kiosk mode (fullscreen, no decorations)");
    parser.addOption(kioskOption);

    QCommandLineOption setupKioskOption("setup-kiosk", "Set up kiosk mode (requires sudo)");
    parser.addOption(setupKioskOption);

    QCommandLineOption undoKioskOption("undo-kiosk", "Undo kiosk setup (requires sudo)");
    parser.addOption(undoKioskOption);

    parser.process(app);

    if (parser.isSet(setupKioskOption)) {
        return setupKiosk(QCoreApplication::applicationFilePath());
    }

    if (parser.isSet(undoKioskOption)) {
        return undoKiosk();
    }

    AppController controller(parser.isSet(kioskOption));
    controller.initialize();

    return app.exec();
}
