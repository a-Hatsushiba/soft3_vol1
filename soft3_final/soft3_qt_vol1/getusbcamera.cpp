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
    Stop = false;

//    putenv("OPENCV_VIDEOIO_MSMF_ENABLE_HW_TRANSFORMS=0");
    if(DEBUG) std::cout << "カメラオープン1" << std::endl;
    cap.open("/dev/video0"); //0：外付け、2：備え付け
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

        /**ここから**/
        cv::Mat gray, ret, ret2;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::adaptiveThreshold(gray, ret, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 11, 5);
        cv::dilate(ret, ret, cv::Mat(), cv::Point(-1,-1), 1);//ココらへんを変える
        cv::erode(ret, ret, cv::Mat(), cv::Point(-1,-1), 1);//ココらへんを変える

        /* 最も外側の輪郭を抽出 */
        std::vector<std::vector<cv::Point>> contours; // 抽出時点の頂点
        cv::findContours(ret, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        cv::cvtColor(ret, ret, cv::COLOR_GRAY2BGR);
        frame.convertTo(ret2, CV_32F, 1.0/255);
        if(DEBUG) std::cout << "外側の輪郭検出完了" << std::endl;

        /* 輪郭を図形に近似 */
//        std::vector<cv::Point2f> approx; // 輪郭の頂点
        for(auto contour = contours.begin(); contour != contours.end(); contour++){
          double epsilon = 0.08 * cv::arcLength(*contour, true);
          cv::approxPolyDP(cv::Mat(*contour), *contour, epsilon, true);
          double area = cv::contourArea(*contour);
          if(DEBUG) std::cout << "近似完了" << std::endl;
          if(area > frame.rows * frame.cols / 2){
//            cv::polylines(dst, std::vector<cv::Point>{*approx}, true, cv::Scalar(0, 0, 255), 2);
            cv::drawContours(dst, std::vector<std::vector<cv::Point>>{*contour}, -1, cv::Scalar(0, 0, 255), 2);
          }
        }
        if(DEBUG) std::cout << "追加プログラム完了" << std::endl;
        /**ここまで**/

        emit valueChangedCam();

        this->msleep(20);
    }
}

void getUsbCamera::getImage(cv::Mat *image)
{
    *image = dst.clone(); //getUsbCameraクラスだからdst使える。
}

void getUsbCamera::getImageForOpenCV(cv::Mat *image)
{
    *image = frame.clone(); //getUsbCameraクラスだからdst使える。
}
