
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QWheelEvent>
#include <filesystem>
#include <qevent.h>

#include "io/array.h"

#ifndef IMG_VIEWER__H
#define IMG_VIEWER__H

enum class PickMode { None, PickP1, PickP2 };

class ImageViewer : public QGraphicsView {
    Q_OBJECT

  public:
    ImageViewer(const tomocam::Array<float> &, QWidget *parent = nullptr);
    void updateImage();
    void updateImageStack(const tomocam::Array<float> &);
    void export_patches(std::filesystem::path);

    // Access picked pixels
    void setPickMode(PickMode mode) { pickMode = mode; }
    QPoint getCenter() const { return center; }
    QPoint getRadius() const { return radius; }
    bool picksReady() const { return pickedCenter && pickedRadius; }
    float get_realCenX() const { return scaleW * center.x(); }
    float get_realCenY() const { return scaleH * center.y(); }
    float get_realRadius() const {
        return std::sqrt(std::pow(scaleW * (radius.x() - center.x()), 2) +
                         std::pow(scaleH * (radius.y() - center.y()), 2));
    }
    // reset center + radius
    void reset() {
        pickedCenter = false;
        pickedRadius = false;
    }

  signals:
    void picksCompleted(QPoint p1, QPoint p2);
    void pickUpdated(int, QPoint);

  public slots:
    void toggle_save_option(bool sw) {
        if (sw)
            save_roi_flag = true;
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
    QPoint center;
    QPoint radius;
    bool pickedCenter;
    bool pickedRadius;
    PickMode pickMode;
    float scaleW;
    float scaleH;
    float realCenX;
    float realCenY;
    float realRmax;

    QImage floatArrayToQImage(const tomocam::Slice<float> &);
};

#endif // IMG_VIEWER__H
