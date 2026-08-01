#include "mainwindow.h"
#include "cameratilewidget.h"
#include "edgemenu.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QCoreApplication>
#include <QApplication>
#include <QCloseEvent>
#include <X11/Xlib.h>
#define XK_MISCELLANY
#include <X11/keysymdef.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      gridLayout_(new QGridLayout),
      kioskMode_(false) {
    QWidget *central = new QWidget;
    central->setLayout(gridLayout_);
    setCentralWidget(central);

    edgeMenu_ = new EdgeMenu(this);
    edgeMenu_->hide();
    connect(edgeMenu_, &EdgeMenu::settingsClicked, this, &MainWindow::settingsRequested);
    connect(edgeMenu_, &EdgeMenu::exitClicked, this, []() { QCoreApplication::quit(); });
}

void MainWindow::setKioskMode(bool kiosk) {
    kioskMode_ = kiosk;
    if (kiosk) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        QApplication::setOverrideCursor(Qt::BlankCursor);
        edgeMenu_->setKioskMode(true);
        showFullScreen();
        grabKioskKeys();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (kioskMode_) {
        event->ignore();
        return;
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::addCamera(const QString &cameraName) {
    CameraTileWidget *tile = new CameraTileWidget(cameraName);
    connect(tile, &CameraTileWidget::retryRequested, this, [this, tile]() {
        int idx = tiles_.indexOf(tile);
        if (idx >= 0) emit retryCamera(idx);
    });
    connect(tile, &CameraTileWidget::menuRequested, this, &MainWindow::onMenuRequested);
    tiles_.append(tile);
    arrangeGrid();
}

void MainWindow::arrangeGrid() {
    gridLayout_->setSpacing(4);
    gridLayout_->setContentsMargins(4, 4, 4, 4);

    int cols = 2;
    int rows = (tiles_.size() + cols - 1) / cols;

    for (int i = 0; i < tiles_.size(); ++i) {
        int row = i / cols;
        int col = i % cols;
        gridLayout_->addWidget(tiles_[i], row, col);
    }
}

void MainWindow::onMenuRequested() {
    if (edgeMenu_->isVisible()) {
        edgeMenu_->hideMenu();
    } else {
        edgeMenu_->move(width() - edgeMenu_->width() - 10, 10);
        edgeMenu_->showMenu();
    }
}

void MainWindow::setCameraFrame(int index, const QImage &frame) {
    if (index >= 0 && index < tiles_.size()) {
        tiles_[index]->setFrame(frame);
    }
}

void MainWindow::setCameraError(int index, const QString &message) {
    if (index >= 0 && index < tiles_.size()) {
        tiles_[index]->setError(message);
    }
}

int MainWindow::cameraCount() const {
    return tiles_.size();
}

static unsigned int modMapMask(Display *d, int index) {
    XModifierKeymap *map = XGetModifierMapping(d);
    unsigned int mask = 0;
    for (int i = 0; i < map->max_keypermod; ++i) {
        if (map->modifiermap[index * map->max_keypermod + i]) {
            mask = 1 << index;
            break;
        }
    }
    XFreeModifiermap(map);
    return mask;
}

void MainWindow::grabKioskKeys() {
    Display *d = XOpenDisplay(nullptr);
    if (!d) return;
    Window root = DefaultRootWindow(d);

    unsigned int mod1 = modMapMask(d, ShiftMapIndex + 1);
    unsigned int ctrl = modMapMask(d, ControlMapIndex);

    XGrabKey(d, XK_Tab, mod1, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(d, XK_Tab, mod1 | ctrl, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(d, XK_Print, 0, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(d, XK_Print, ctrl, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(d, XK_Sys_Req, 0, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(d, XK_Sys_Req, ctrl, root, True, GrabModeAsync, GrabModeAsync);

    KeySym superKeys[] = {XK_Super_L, XK_Super_R};
    for (KeySym sym : superKeys) {
        KeyCode code = XKeysymToKeycode(d, sym);
        if (code) {
            XGrabKey(d, code, AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
        }
    }

    XSync(d, False);
    XCloseDisplay(d);
}
