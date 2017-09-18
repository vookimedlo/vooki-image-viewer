#pragma once

#include <cstdint>
#include <QWidget>
#include "../util/RotatingIndex.h"

class ImageArea : public QWidget
{
    Q_OBJECT

public:
    ImageArea(QWidget *parent = 0);

    void setFitToWindow(bool enabled);
    bool showImage(const QString &fileName);
    void rotateLeft();
    void rotateRight();
    void zoomImageIn(double factor);
    void zoomImageOut(double factor);
    void zoomReset();

public slots:

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void transformImage();

private:
    bool m_isFitToWindow;
    double m_scaleFactor;
    QImage m_originalImage;
    QImage m_finalImage;
    RotatingIndex<uint8_t> m_rotateIndex;
};
