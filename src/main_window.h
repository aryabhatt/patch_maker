
#ifndef MAIN_WINDOW__H
#define MAIN_WINDOW__H
#include <QMainWindow>
#include <filesystem>

#include "image_viewer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    int maxWidth() const { return maxW; }
    int maxHeight() const { return maxH; }

  private slots:
    void openFile();
    void export_patches();
    void onPicksCompleted(QPoint, QPoint);
    void onPickUpdated(int, QPoint);

  private:
    std::filesystem::path subdir_name;
    ImageViewer *viewer;
    QAction *exportAction;
    QAction *pick1Action;
    QAction *pick2Action;
    QAction *resetAction;
    int maxW;
    int maxH;
};

#endif // MAIN_WINDOW__H
