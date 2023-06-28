#include "getusbcamera.h"

#define DEBUG 0
/** USBカメラ **/
getUsbCamera::getUsbCamera(QObject *parent, bool b) : QThread(parent), Stop(b)
{

}

getUsbCamera::~getUsbCamera()
{
    Stop = true;
}

bool getUsbCamera::initCam(void) //カメラ読み込みできるか
{
    bool ret = true;

//    putenv("OPENCV_VIDEOIO_MSMF_ENABLE_HW_TRANSFORMS=0");
    if(DEBUG) std::cout << "カメラオープン1" << std::endl;
    cap.open(0); //0：外付け、2：備え付け
    if(DEBUG) std::cout << "カメラオープン2" << std::endl;
    if(cap.isOpened()){
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    }else{
        ret = false;
    }
    if(DEBUG) std::cout << "カメラオープン3" << std::endl;
    return ret;
}

void getUsbCamera::run(void) //実行
{
    QMutex mutex;

    while(1){
        mutex.lock();
        if(this->Stop) break;
        mutex.unlock();

        cap >> frame; //カメラからフレームを取得
        cv::cvtColor(frame, dst, cv::COLOR_BGR2RGB);

        emit valueChangedCam();

        this->msleep(20);
    }
}

void getUsbCamera::getImage(cv::Mat *image)
{
    *image = dst.clone(); //getUsbCameraクラスだからdst使える。
}
