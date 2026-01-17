#include <QFileDialog>
#include <QGuiApplication>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>
#include <QStatusBar>
#include <QToolBar>
#include <filesystem>
#include <qaction.h>
#include <qdialog.h>
#include <qmenu.h>
#include <qpushbutton.h>

#include "io/array.h"
#include "io/loader.h"
#include "main_window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    tomocam::Array<float> img0(1, 1, 1);
    viewer = new ImageViewer(img0, this); // Start with empty stack
    setCentralWidget(viewer);

    // get monitor screen size
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect geo = screen->availableGeometry();
        maxW = static_cast<int>(geo.width() * 0.5);
        maxH = static_cast<int>(geo.height() * 0.75);
        resize(maxW, maxH);
    }

    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *openAction = fileMenu->addAction("&Open");
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    exportAction = fileMenu->addAction("&Export Patches");
    connect(exportAction, &QAction::triggered, this, &MainWindow::export_patches);
    exportAction->setEnabled(false);

    // Toolbar
    QToolBar *toolbar = addToolBar("&Tools");
    pick1Action = toolbar->addAction("&Set Center");
    pick1Action->setEnabled(false);
    pick2Action = toolbar->addAction("&Set Radius");
    pick2Action->setEnabled(false);

    connect(pick1Action, &QAction::triggered, [this]() {
        viewer->setPickMode(PickMode::PickP1);
        statusBar()->showMessage("Pick approximate center");
    });
    connect(pick2Action, &QAction::triggered, [this]() {
        viewer->setPickMode(PickMode::PickP2);
        statusBar()->showMessage("Pick approximate radius");
    });
    resetAction = toolbar->addAction("&Reset");
    resetAction->setEnabled(false);
    connect(resetAction, &QAction::triggered, this, [this]() {
        pick1Action->setEnabled(true);
        pick2Action->setEnabled(true);
        exportAction->setEnabled(false);
        viewer->reset();
        statusBar()->showMessage("Ready");
    });

    // show picked points in statusbar
    statusBar()->showMessage("Ready");
    connect(viewer, &ImageViewer::pickUpdated, this, &MainWindow::onPickUpdated);
    connect(viewer, &ImageViewer::picksCompleted, this, &MainWindow::onPicksCompleted);
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image Stack", "",
                                                    "TIFF Files (*.tif *.tiff);; HD5 Files (*.h5)");
    if (fileName.isEmpty())
        return;

    auto filename = fileName.toStdString();
    auto data = tomocam::loader(fileName.toStdString());
    auto width = data.ncols();
    auto height = data.nrows();
    auto depth = data.nslices();

    if (width < 0 || height < 0 || depth < 0) {
        QMessageBox::critical(this, "Error", "Failed to open file");
        return;
    }
    viewer->updateImageStack(data);
    // turn on all the buttons
    pick1Action->setEnabled(true);
    pick2Action->setEnabled(true);
    resetAction->setEnabled(true);
    subdir_name = std::filesystem::path(filename).stem();
}

void MainWindow::onPickUpdated(int which, QPoint pt) {
    if (which == 1) {
        statusBar()->showMessage(QString("Center: (%1, %2)").arg(pt.x()).arg(pt.y()));
        pick1Action->setEnabled(false);
    } else if (which == 2) {
        statusBar()->showMessage(QString("Radius: (%1, %2)").arg(pt.x()).arg(pt.y()));
        pick2Action->setEnabled(false);
    }
}

void MainWindow::onPicksCompleted(QPoint p1, QPoint p2) {
    statusBar()->showMessage(QString("Center: (%1, %2) | Radius: (%3, %4)")
                                 .arg(p1.x())
                                 .arg(p1.y())
                                 .arg(p2.x())
                                 .arg(p2.y()));
    exportAction->setEnabled(true);
}

void MainWindow::export_patches() {
    if (!std::filesystem::is_directory(subdir_name)) {
        std::filesystem::create_directory(subdir_name);
    }
    viewer->export_patches(subdir_name);
}
