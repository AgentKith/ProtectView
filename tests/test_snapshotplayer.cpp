#include <QtTest>
#include "video/snapshotplayer.h"

class TestSnapshotPlayer : public QObject {
    Q_OBJECT
private slots:
    void testConstruction() {
        SnapshotPlayer player("https://192.168.1.100/snapshot");
        QVERIFY(true);
    }

    void testConstructionWithInterval() {
        SnapshotPlayer player("https://192.168.1.100/snapshot", 500);
        QVERIFY(true);
    }

    void testStartStop() {
        SnapshotPlayer player("https://192.168.1.100/snapshot");
        player.start();
        QTest::qSleep(100);
        player.stop();
        QVERIFY(true);
    }
};

QTEST_MAIN(TestSnapshotPlayer)
#include "test_snapshotplayer.moc"
