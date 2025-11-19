#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <algorithm>
#include <cstdint>
#include <qevent.h>
#include <qgraphicsview.h>
#include <qnamespace.h>
#include <string>
#include <unistd.h>

#include "image_viewer.h"
#include "io/tiff/tiffio.h"

ImageViewer::ImageViewer(const tomocam::Array<float> &images, QWidget *parent) :
    QGraphicsView(parent),
    imageStack(images),
    currentIndex(0) {
    scene = new QGraphicsScene(this);
    setScene(scene);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setFocusPolicy(Qt::StrongFocus);
    setBackgroundBrush(QBrush(Qt::black));
    updateImage();
}

void ImageViewer::updateImage() {
    scene->clear();
    QImage img = floatArrayToQImage(imageStack.slice(currentIndex));
    scene->addPixmap(QPixmap::fromImage(img));
    scene->setSceneRect(img.rect());
    
    setWindowTitle(QString("Page %1/%2").arg(currentIndex + 1).arg(imageStack.nslices()));
}

QImage ImageViewer::floatArrayToQImage(const tomocam::Slice<float> &array) {
    int h = array.nrows;
    int w = array.ncols;

    // Find min/max for normalization and rescaling to unsigned int
    float minVal = array[0];
    float maxVal = array[0];
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            float val = array[y * w + x];
            minVal = std::min(minVal, val);
            maxVal = std::max(maxVal, val);
        }
    }

    // Create grayscale image by rescaling float32 to uint8
    QImage img(w, h, QImage::Format_Grayscale8);
    float range = maxVal - minVal;
    
    if (range > 0.0f) {
        for (int y = 0; y < h; ++y) {
            uchar *line = img.scanLine(y);
            for (int x = 0; x < w; ++x) {
                // Rescale float32 to unsigned int (0-255 for display)
                float normalized = (array[y * w + x] - minVal) / range;
                line[x] = static_cast<uchar>(normalized * 255.0f);
            }
        }
    } else {
        // Handle constant-value images
        img.fill(128);
    }
    
    return img;
}

void ImageViewer::wheelEvent(QWheelEvent *event) {
    auto nImgs = imageStack.nslices();

    int step = 1;
    if (event->modifiers() & Qt::ControlModifier) { step = 5; }

    if (event->angleDelta().y() > 0) {
        currentIndex = (currentIndex + step) % nImgs;
    } else {
        currentIndex = (currentIndex - step + nImgs) % nImgs;
    }
    updateImage();
}

void ImageViewer::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
}

void ImageViewer::updateImageStack(const tomocam::Array<float> &arr) {
    imageStack = arr;
    currentIndex = 0;
    updateImage();
}

void ImageViewer::keyPressEvent(QKeyEvent *event) {

    if (imageStack.size() <= 0) {
        QGraphicsView::keyReleaseEvent(event);
        return;
    }

    int nImgs = imageStack.nslices();
    int oldIndex = currentIndex;
    switch (event->key()) {
        case Qt::Key_Up: currentIndex = (currentIndex + 1) % nImgs; break;
        case Qt::Key_Down:
            currentIndex = (currentIndex - 1 + nImgs) % nImgs;
            break;
        case Qt::Key_PageUp: currentIndex = (currentIndex + 5) % nImgs; break;
        case Qt::Key_PageDown: currentIndex = (currentIndex - 5) % nImgs; break;
        case Qt::Key_Home: currentIndex = 0; break;
        case Qt::Key_End: currentIndex = nImgs - 1; break;
        default: QGraphicsView::keyPressEvent(event); return;
    }

    if (currentIndex != oldIndex) { updateImage(); }
}
