#include <QApplication>
#include "app/appcontroller.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("unvr-carousal");
    app.setOrganizationName("unvr-carousal");

    AppController controller;
    controller.initialize();

    return app.exec();
}
