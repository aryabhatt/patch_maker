
#ifndef MAIN_WINDOW__H
#define MAIN_WINDOW__H
#include <QMainWindow>

#include "image_viewer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    MainWindow(QWidget *parent = nullptr);

  private slots:
    void openFile();
    void toggle_save(bool);

  private:
    ImageViewer *viewer;
};

#endif // MAIN_WINDOW__H
