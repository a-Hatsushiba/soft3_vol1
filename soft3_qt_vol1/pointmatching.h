#pragma once
#ifndef POINTMATCHING_H
#define POINTMATCHING_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <numeric>
#include <QObject>
#include <QThread>
#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>

//extern std::string g_img_path1, g_img_path2; //ファイル画像のパスを入れるためのグローバル変数
//std::string g_result_path1, g_result_path2;

class pointMatching : public QThread
{
    Q_OBJECT
public:
    pointMatching(QObject *parent = 0, bool b = false);
    ~pointMatching();
    void run(void);
signals:
    void nextPage();
    void errorPage();
private:
    int feature_matching(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst);
    void absdiff1(const cv::Mat &src, const cv::Mat &dst);
    void absdiff2(const cv::Mat &src, const cv::Mat &dst);
    bool Stop;
};

#endif // POINTMATCHING_H
