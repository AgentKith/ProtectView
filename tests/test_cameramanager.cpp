#include <QtTest>
#include <QApplication>
#include "app/cameramanager.h"
#include "unvr/client.h"

class TestCameraManager : public QObject {
    Q_OBJECT
private slots:
    void testConstruction() {
        AppConfig config;
        UNVRClient client("192.168.1.100", "test-key");
        CameraManager manager(config, &client);
        QCOMPARE(manager.cameraCount(), 0);
    }

    void testSetCameras() {
        AppConfig config;
        UNVRClient client("192.168.1.100", "test-key");
        CameraManager manager(config, &client);

        QList<CameraInfo> cameras;
        CameraInfo cam1;
        cam1.id = "cam1";
        cam1.name = "Front Door";
        cam1.enabled = true;
        cameras.append(cam1);

        CameraInfo cam2;
        cam2.id = "cam2";
        cam2.name = "Back Door";
        cam2.enabled = true;
        cameras.append(cam2);

        manager.setCameras(cameras);
        QCOMPARE(manager.cameraCount(), 2);
        QCOMPARE(manager.cameraName(0), "Front Door");
        QCOMPARE(manager.cameraName(1), "Back Door");
    }

    void testCameraNameOutOfBounds() {
        AppConfig config;
        UNVRClient client("192.168.1.100", "test-key");
        CameraManager manager(config, &client);
        QVERIFY(manager.cameraName(-1).isEmpty());
        QVERIFY(manager.cameraName(0).isEmpty());
    }

    void testStopAllEmpty() {
        AppConfig config;
        UNVRClient client("192.168.1.100", "test-key");
        CameraManager manager(config, &client);
        manager.stopAll();
        QVERIFY(true);
    }
};

QTEST_MAIN(TestCameraManager)
#include "test_cameramanager.moc"
