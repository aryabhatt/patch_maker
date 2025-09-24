#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <qaction.h>
#include <qdialog.h>
#include <qmenu.h>
#include <qpushbutton.h>

#include "io/array.h"
#include "io/tiff/tiffio.h"
#include "main_window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    tomocam::Array<float> img0(1, 512, 512);
    viewer = new ImageViewer(img0, this); // Start with empty stack
    setCentralWidget(viewer);

    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *openAction = fileMenu->addAction("&Open");
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    QAction *checkAction = fileMenu->addAction("&Save ROI");
    checkAction->setCheckable(true);
    connect(checkAction, &QAction::toggled, this, &MainWindow::toggle_save);
}

void MainWindow::toggle_save(bool sw) {
    if (sw) {
        viewer->toggle_save_option(true);
    } else {
        viewer->toggle_save_option(false);
    }
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image Stack",
        "", "TIFF Files (*.tif *.tiff)");
    if (fileName.isEmpty()) return;
    auto data = tomocam::tiff::read<float>(fileName.toStdString());

    auto width = data.ncols();
    auto height = data.nrows();
    auto depth = data.nslices();

    if (width < 0 || height < 0 || depth < 0) {
        QMessageBox::critical(this, "Error", "Failed to open file");
        return;
    }
    viewer->updateImageStack(data);
}
