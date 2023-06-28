#ifndef GETUSBCAMERA_H
#define GETUSBCAMERA_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <opencv2/opencv.hpp>

class getUsbCamera : public QThread
{
    Q_OBJECT
public:
    getUsbCamera(QObject *parent = 0, bool b = false);
    ~getUsbCamera();
    bool initCam(void);
    void run(void);
    void getImage(cv::Mat*);
    bool Stop;
signals:
    void valueChangedCam(void);
private:
    cv::VideoCapture cap;
    cv::Mat frame, dst;
};

#endif // GETUSBCAMERA_H
