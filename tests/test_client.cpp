#include <QtTest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "unvr/types.h"

class TestClient : public QObject {
    Q_OBJECT
private slots:
    void testCameraInfoFromJson() {
        QJsonObject obj;
        obj["id"] = "cam1";
        obj["name"] = "Front Door";
        obj["type"] = "UVC G3 Flex";
        obj["width"] = 1920;
        obj["height"] = 1080;
        obj["codec"] = "H264";
        obj["enabled"] = true;

        CameraInfo cam = CameraInfo::fromJson(obj);

        QCOMPARE(cam.id, "cam1");
        QCOMPARE(cam.name, "Front Door");
        QCOMPARE(cam.type, "UVC G3 Flex");
        QCOMPARE(cam.width, "1920");
        QCOMPARE(cam.height, "1080");
        QCOMPARE(cam.codec, "H264");
        QVERIFY(cam.enabled);
    }

    void testCameraInfoFromJsonDefaults() {
        QJsonObject obj;
        obj["id"] = "cam1";

        CameraInfo cam = CameraInfo::fromJson(obj);

        QCOMPARE(cam.id, "cam1");
        QVERIFY(cam.name.isEmpty());
        QVERIFY(cam.enabled);
    }

    void testStreamResponseFromJson() {
        QJsonObject obj;
        obj["high"] = "rtsps://192.168.1.100:7442/stream-high";
        obj["medium"] = "rtsps://192.168.1.100:7442/stream-medium";
        obj["low"] = "rtsps://192.168.1.100:7442/stream-low";
        obj["package"] = "rtsps://192.168.1.100:7442/stream-package";

        StreamResponse stream = StreamResponse::fromJson(obj);

        QCOMPARE(stream.high, "rtsps://192.168.1.100:7442/stream-high");
        QCOMPARE(stream.medium, "rtsps://192.168.1.100:7442/stream-medium");
        QCOMPARE(stream.low, "rtsps://192.168.1.100:7442/stream-low");
        QCOMPARE(stream.package, "rtsps://192.168.1.100:7442/stream-package");
    }

    void testCameraInfoToJson() {
        CameraInfo cam;
        cam.id = "cam1";
        cam.name = "Front Door";
        cam.type = "UVC G3 Flex";
        cam.width = "1920";
        cam.height = "1080";
        cam.codec = "H264";
        cam.enabled = true;

        QJsonObject obj = cam.toJson();

        QCOMPARE(obj["id"].toString(), "cam1");
        QCOMPARE(obj["name"].toString(), "Front Door");
        QCOMPARE(obj["type"].toString(), "UVC G3 Flex");
        QCOMPARE(obj["width"].toString(), "1920");
        QCOMPARE(obj["height"].toString(), "1080");
        QCOMPARE(obj["codec"].toString(), "H264");
        QVERIFY(obj["enabled"].toBool());
    }

    void testCameraInfoRoundtrip() {
        CameraInfo original;
        original.id = "cam1";
        original.name = "Front Door";
        original.type = "UVC G3 Flex";
        original.width = "1920";
        original.height = "1080";
        original.codec = "H264";
        original.enabled = true;

        QJsonObject obj = original.toJson();
        CameraInfo roundtrip = CameraInfo::fromJson(obj);

        QCOMPARE(roundtrip.id, original.id);
        QCOMPARE(roundtrip.name, original.name);
        QCOMPARE(roundtrip.type, original.type);
        QCOMPARE(roundtrip.width, original.width);
        QCOMPARE(roundtrip.height, original.height);
        QCOMPARE(roundtrip.codec, original.codec);
        QCOMPARE(roundtrip.enabled, original.enabled);
    }

    void testCameraInfoDisabled() {
        QJsonObject obj;
        obj["id"] = "cam1";
        obj["enabled"] = false;

        CameraInfo cam = CameraInfo::fromJson(obj);

        QCOMPARE(cam.id, "cam1");
        QVERIFY(!cam.enabled);
    }
};

QTEST_MAIN(TestClient)
#include "test_client.moc"
