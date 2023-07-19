#ifndef GETUSBCAMERA_H
#define GETUSBCAMERA_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>


class getUsbCamera : public QThread
{
    Q_OBJECT
public:
    getUsbCamera(QObject *parent = 0, bool b = false);
    ~getUsbCamera();
    bool initCam(void);
    void run(void);
    void getImage(cv::Mat*);
    void getImageForOpenCV(cv::Mat *image);
    bool Stop;
signals:
    void valueChangedCam(void);
private:
    cv::VideoCapture cap = cv::VideoCapture(0, cv::CAP_DSHOW);
    cv::Mat frame, dst;
};

#endif // GETUSBCAMERA_H
