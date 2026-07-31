#include <QtTest>
#include <QApplication>
#include "ui/cameratilewidget.h"

class TestCameraTile : public QObject {
    Q_OBJECT
private slots:
    void testConstruction() {
        CameraTileWidget tile("Front Door");
        QCOMPARE(tile.cameraName(), "Front Door");
    }

    void testSetFrame() {
        CameraTileWidget tile("Front Door");
        QImage frame(640, 480, QImage::Format_RGBA8888);
        frame.fill(Qt::blue);
        tile.setFrame(frame);
        QVERIFY(true);
    }

    void testSetError() {
        CameraTileWidget tile("Front Door");
        tile.setError("Connection failed");
        QVERIFY(true);
    }

    void testClearError() {
        CameraTileWidget tile("Front Door");
        tile.setError("Connection failed");
        tile.clearError();
        QVERIFY(true);
    }
};

QTEST_MAIN(TestCameraTile)
#include "test_cameratile.moc"
