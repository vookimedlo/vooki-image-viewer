#ifndef IMAGEAREA_H
#define IMAGEAREA_H

#include <QWidget>

class ImageArea : public QWidget
{
    Q_OBJECT

private:
    bool isFitToWindow;
    double scaleFactor;
    QImage originalImage;
    QImage scaledImage;
    QImage finalImage;

public:
    ImageArea(QWidget *parent = 0);

    void setFitToWindow(bool enabled);
    bool showImage(const QString &fileName);
    void zoomImageIn(double factor);
    void zoomImageOut(double factor);

public slots:

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void scaleImage();
};

#endif // IMAGEAREA_H
