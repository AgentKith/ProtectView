#include "kiosk.h"
#include <QCoreApplication>
#include <QFile>
#include <QProcess>

static int runScript(const QString &resourcePath, const QStringList &args) {
    QString scriptPath = QCoreApplication::applicationDirPath() + "/.kiosk-temp.sh";

    QFile script(scriptPath);
    QFile resource(resourcePath);
    resource.open(QIODevice::ReadOnly);
    script.open(QIODevice::WriteOnly);
    script.write(resource.readAll());
    script.close();
    resource.close();
    script.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                          QFileDevice::ExeOwner | QFileDevice::ReadGroup |
                          QFileDevice::ExeGroup | QFileDevice::ReadOther |
                          QFileDevice::ExeOther);

    int exitCode = QProcess::execute(scriptPath, args);
    QFile::remove(scriptPath);
    return exitCode;
}

int setupKiosk(const QString &binaryPath) {
    return runScript(":/kiosk-setup.sh", {binaryPath});
}

int undoKiosk() {
    return runScript(":/kiosk-undo.sh", {});
}
