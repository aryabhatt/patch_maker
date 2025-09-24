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
    currentIndex(0),
    counter(0),
    save_roi_flag(false) {
    scene = new QGraphicsScene(this);
    setScene(scene);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setFocusPolicy(Qt::StrongFocus);
    updateImage();
}

void ImageViewer::updateImage() {
    scene->clear();
    QImage img = floatArrayToQImage(imageStack.slice(currentIndex));
    scene->addPixmap(QPixmap::fromImage(img));
    scene->setSceneRect(img.rect());
}

QImage ImageViewer::floatArrayToQImage(const tomocam::Slice<float> &array) {
    int h = array.nrows;
    int w = array.ncols;

    // Find min/max for normalization
    minVal = array[0];
    maxVal = array[0];
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            minVal = std::min(minVal, array[y * w + x]);
            maxVal = std::max(maxVal, array[y * w + x]);
        }
    }

    QImage img(w, h, QImage::Format_Grayscale8);
    for (int y = 0; y < h; ++y) {
        uchar *line = img.scanLine(y);
        for (int x = 0; x < w; ++x) {
            float norm = (array[y * w + x] - minVal) / (maxVal - minVal);
            line[x] = static_cast<uchar>(norm * 255.0f);
        }
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
    QPointF scenePos = mapToScene(event->pos());
    int x = static_cast<int>(scenePos.x());
    int y = static_cast<int>(scenePos.y());

    if (x >= 0 && y >= 0 && y < static_cast<int>(imageStack.nrows()) &&
        x < static_cast<int>(imageStack.ncols())) {
        saveROI(currentIndex, y, x);
    }
    QGraphicsView::mousePressEvent(event);
}

void ImageViewer::updateImageStack(const tomocam::Array<float> &arr) {
    imageStack = arr;
    currentIndex = 0;
    updateImage();
}

void ImageViewer::saveROI(int z, int y, int x) {

    if (!save_roi_flag) return;

    uint32_t h = 256;
    uint32_t w = 256;
    if (y - h / 2 < 0 || y + h / 2 >= imageStack.nrows() || x - w / 2 < 0 ||
        x + w / 2 >= imageStack.ncols()) {
        return;
    }
    tomocam::Array<uint32_t> box(1, h, w);

    uint32_t slc = (uint32_t)z;
    uint32_t ibeg = y - h / 2;
    uint32_t jbeg = x - h / 2;
    for (uint32_t i = 0; i < h; ++i) {
        for (uint32_t j = 0; j < w; ++j) {
            float pxlval = imageStack[{slc, ibeg + i, jbeg + j}];
            box[{0, i, j}] = static_cast<uint32_t>(
                255.0 * (pxlval - minVal) / (maxVal - minVal));
        }
    }
    char path[20];
    sprintf(path, "patch%05d.tif", counter);
    tomocam::tiff::write<uint32_t>(std::string(path), box);
    counter += 1;
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
