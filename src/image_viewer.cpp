#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <qevent.h>
#include <qgraphicsview.h>
#include <qnamespace.h>
#include <unistd.h>

#include "image_viewer.h"
#include "io/tiff/tiffio.h"
#include "main_window.h"
#include "save_patch.h"

constexpr int PATCHES_PER_FRAME = 1;

template <typename T> T random() { return static_cast<T>(rand()) / static_cast<T>(RAND_MAX); }

ImageViewer::ImageViewer(const tomocam::Array<float> &images, QWidget *parent)
    : QGraphicsView(parent), imageStack(images), currentIndex(0), counter(0), save_roi_flag(false),
      pickedCenter(false), pickedRadius(false), pickMode(PickMode::None), scaleH(1.f), scaleW(1.f) {

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
    // resize of image is too big
    // get main window
    MainWindow *mainWin;
    QWidget *curr_parent = this->parentWidget();
    while (curr_parent) {
        mainWin = qobject_cast<MainWindow *>(curr_parent);
        if (mainWin) {
            break;
        }
    }
    if (mainWin) {
        if (h > mainWin->maxHeight() || w > mainWin->maxWidth()) {
            img = img.scaled(mainWin->maxWidth(), mainWin->maxHeight(),
                             Qt::KeepAspectRatioByExpanding);
            scaleH = static_cast<float>(h) / static_cast<float>(mainWin->maxHeight());
            scaleW = static_cast<float>(w) / static_cast<float>(mainWin->maxWidth());
        }
    }
    return img;
}

void ImageViewer::wheelEvent(QWheelEvent *event) {
    auto nImgs = imageStack.nslices();

    int step = 1;
    if (event->modifiers() & Qt::ControlModifier) {
        step = 5;
    }

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

    if (imageStack.size() > 0 && x >= 0 && y >= 0 && y < static_cast<int>(imageStack.nrows()) &&
        x < static_cast<int>(imageStack.ncols())) {
        if (pickMode == PickMode::PickP1) {
            center = QPoint(x, y);
            pickedCenter = true;
            emit pickUpdated(1, center);
            pickMode = PickMode::None;
        } else if (pickMode == PickMode::PickP2) {
            radius = QPoint(x, y);
            pickedRadius = true;
            emit pickUpdated(2, radius);
            pickMode = PickMode::None;
        }
        if (pickedCenter && pickedRadius) {
            emit picksCompleted(center, radius);
        }
    }
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
    case Qt::Key_Up:
        currentIndex = (currentIndex + 1) % nImgs;
        break;
    case Qt::Key_Down:
        currentIndex = (currentIndex - 1 + nImgs) % nImgs;
        break;
    case Qt::Key_PageUp:
        currentIndex = (currentIndex + 5) % nImgs;
        break;
    case Qt::Key_PageDown:
        currentIndex = (currentIndex - 5) % nImgs;
        break;
    case Qt::Key_Home:
        currentIndex = 0;
        break;
    case Qt::Key_End:
        currentIndex = nImgs - 1;
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        return;
    }

    if (currentIndex != oldIndex) {
        updateImage();
    }
}

void ImageViewer::export_patches(std::filesystem::path subdir) {

    float maxR = get_realRadius();
    float cenX = get_realCenX();
    float cenY = get_realCenY();

    float constexpr SECTOR_SIZE = 2 * M_PI / PATCHES_PER_FRAME;
    for (int i = 0; i < imageStack.nslices(); i++) {
        for (int j = 0; j < PATCHES_PER_FRAME; j++) {
            float t = (j + random<float>()) * SECTOR_SIZE;
            float r = random<float>() * maxR;
            float x = cenX + r * std::cos(t);
            float y = cenY + r * std::sin(t);
            char pname[20];
            snprintf(pname, 20, "%05d.tif", counter);
            auto tifname = std::filesystem::path(pname);
            tomocam::dims_t loc{(uint32_t)i, (uint32_t)y, (uint32_t)x};
            save_patch((subdir / tifname).string(), imageStack, loc);
            counter += 1;
        }
    }
}
