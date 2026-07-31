#include <QtTest>
#include "video/ffmpegplayer.h"

class TestFFmpegPlayer : public QObject {
    Q_OBJECT
private slots:
    void testConstruction() {
        FFmpegPlayer player("rtsps://192.168.1.100/stream");
        QVERIFY(true);
    }

    void testStopWithoutStart() {
        FFmpegPlayer player("rtsps://192.168.1.100/stream");
        player.stop();
        QVERIFY(true);
    }
};

QTEST_MAIN(TestFFmpegPlayer)
#include "test_ffmpegplayer.moc"
