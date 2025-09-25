
#ifndef MAIN_WINDOW__H
#define MAIN_WINDOW__H
#include <QMainWindow>

#include "image_viewer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    MainWindow(QWidget *parent = nullptr);
    int maxWidth() const { return maxW; }
    int maxHeight() const { return maxH; }

  private slots:
    void openFile();
    void toggle_save(bool);

  private:
    ImageViewer *viewer;
    int maxW;
    int maxH;
};

#endif // MAIN_WINDOW__H
