#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <unistd.h>
/** USBカメラ用 **/
#include <QMessageBox>
#include <QThread>
#include "getusbcamera.h"
/** BGM用 **/
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QApplication>
#include <sstream>
/*** 自力モード用(廣瀬担当) ***/
#include <QStackedWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QPixmap>
#include <QtCore/QDebug>
#include <QLabel>
#include <QImage>
#include <QSoundEffect>
#include <unistd.h>
/*** 画像処理用 ***/
#include "pointmatching.h"

//std::string g_img_path1, g_img_path2; //ファイル画像のパスを入れるためのグローバル変数
//extern std::string g_result_path1, g_result_path2;//結果表示画面の画像ファイルパスとエラーのときいを入れてもらうやつ

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    getUsbCamera *getUsbCamT; //usbカメラ追加
    pointMatching *pointMatch; //画像処理用のオブジェクト
private slots:
    /** 初芝担当箇所 **/
    void on_quitButton_released();
    void on_startButton_released();
    void on_fileSelectButton_released();
    void on_returnButton_1_released();
    void on_fileOpenButton_1_released();
    void on_returnButton_2_released();
    void onValueChangedCam(void); //USBカメラ
    void on_cameraButton_1_released();
    void on_cameraTakeButton_1_released();
    void on_fileOpenButton_2_released();
    void moveNextPage();
    void moveErrorPage();
    void on_progressBar_valueChanged(int value);
    /*** 廣瀬担当箇所 ***/
    void on_startbutton_released();
    void updateTime();
    void on_radioButton_clicked(bool checked);
    void on_giveupbutton_released();
    void mousePressEvent(QMouseEvent *event);
    void on_clearbutton_released();
    /*** 浅川担当箇所 ***/
    void on_select_cheat_released();
    void on_select_jiriki_released();


    void on_start_return_Button_released();
    void on_startReturnButton_2_released();


private:
    Ui::MainWindow *ui;

    /*** USBカメラ用 ***/
    QImage qtImage;

    /*** 自力モード用 ***/
    QTimer *timer;
    int num = 10;//間違いの個数
    unsigned long int time10 = 10000; //制限時間1000=10秒、間違いの個数、難易度によって変化？
    QList<QLabel*> imageLabels;
};
#endif // MAINWINDOW_H
