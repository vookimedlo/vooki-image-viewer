#pragma once

#include <QWidget>

class ImageArea : public QWidget
{
    Q_OBJECT

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

private:
    bool isFitToWindow;
    double scaleFactor;
    QImage originalImage;
    QImage scaledImage;
    QImage finalImage;
};
