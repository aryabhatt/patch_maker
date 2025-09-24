#include <QApplication>

#include "image_viewer.h"
#include "io/array.h"
#include "main_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Example: Create a 3D float array: depth x height x width
    MainWindow win;
    win.resize(512, 512);
    win.show();

    return app.exec();
}
