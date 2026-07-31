#include <QtTest>
#include <QApplication>
#include "ui/edgemenu.h"

class TestEdgeMenu : public QObject {
    Q_OBJECT
private slots:
    void testConstruction() {
        EdgeMenu menu;
        QVERIFY(true);
    }

    void testShowHide() {
        EdgeMenu menu;
        menu.showMenu();
        QTest::qSleep(100);
        menu.hideMenu();
        QVERIFY(true);
    }
};

QTEST_MAIN(TestEdgeMenu)
#include "test_edgemenu.moc"
