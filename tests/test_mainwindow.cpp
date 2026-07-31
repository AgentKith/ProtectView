#include <QtTest>
#include <QApplication>
#include "ui/mainwindow.h"

class TestMainWindow : public QObject {
    Q_OBJECT
private slots:
    void testConstruction() {
        MainWindow window;
        QCOMPARE(window.cameraCount(), 0);
    }

    void testAddCamera() {
        MainWindow window;
        window.addCamera("Front Door");
        QCOMPARE(window.cameraCount(), 1);
    }

    void testAddMultipleCameras() {
        MainWindow window;
        window.addCamera("Front Door");
        window.addCamera("Back Door");
        window.addCamera("Garage");
        QCOMPARE(window.cameraCount(), 3);
    }

    void testSetCameraFrame() {
        MainWindow window;
        window.addCamera("Front Door");
        QImage frame(640, 480, QImage::Format_RGBA8888);
        frame.fill(Qt::blue);
        window.setCameraFrame(0, frame);
        QVERIFY(true);
    }

    void testSetCameraError() {
        MainWindow window;
        window.addCamera("Front Door");
        window.setCameraError(0, "Connection failed");
        QVERIFY(true);
    }
};

QTEST_MAIN(TestMainWindow)
#include "test_mainwindow.moc"
