#include "fingerprint.h"

#include <QFile>
#include <QTextStream>

QString getDeviceFingerprint() {
    QFile cpuinfo("/proc/cpuinfo");
    if (cpuinfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&cpuinfo);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("Serial")) {
                int idx = line.indexOf(':');
                if (idx != -1) {
                    QString serial = line.mid(idx + 1).trimmed();
                    if (!serial.isEmpty()) {
                        return serial;
                    }
                }
            }
        }
    }

    QFile machineId("/etc/machine-id");
    if (machineId.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&machineId);
        QString id = in.readLine().trimmed();
        if (!id.isEmpty()) {
            return id;
        }
    }

    return {};
}
