
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QWheelEvent>
#include <qevent.h>

#include "io/array.h"

#ifndef IMG_VIEWER__H
#define IMG_VIEWER__H

class ImageViewer : public QGraphicsView {
    Q_OBJECT

  public:
    ImageViewer(const tomocam::Array<float> &, QWidget *parent = nullptr);
    void updateImage();
    void updateImageStack(const tomocam::Array<float> &);
    void saveROI(int, int, int);

  public slots:
    void toggle_save_option(bool sw) {
        if (sw) save_roi_flag = true;
        else
            save_roi_flag = false;
    }

  protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

  private:
    QGraphicsScene *scene;
    tomocam::Array<float> imageStack;
    int currentIndex;
    int counter;
    bool save_roi_flag;
    float minVal;
    float maxVal;

    QImage floatArrayToQImage(const tomocam::Slice<float> &);
};

#endif // IMG_VIEWER__H
