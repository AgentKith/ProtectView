#include "mainwindow.h"
#include "cameratilewidget.h"
#include "edgemenu.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      gridLayout_(new QGridLayout) {
    QWidget *central = new QWidget;
    central->setLayout(gridLayout_);
    setCentralWidget(central);

    edgeMenu_ = new EdgeMenu(this);
    edgeMenu_->hide();
    connect(edgeMenu_, &EdgeMenu::settingsClicked, this, &MainWindow::settingsRequested);
    connect(edgeMenu_, &EdgeMenu::exitClicked, this, []() { QCoreApplication::quit(); });
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
