#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DEBUG 0
#define VIEWPATH 0
//#define PATH1 "/home/akari/workspace/soft3/Qt/soft3_vol2/test1.png"
//#define PATH2 "/home/akari/workspace/soft3/Qt/soft3_vol2/test2.png"
/*** ファイル画像のパスを入れるためのグローバル変数 ***/
extern std::string g_img_path1, g_img_path2;
/*** 結果表示画面の画像ファイルパスとエラーのときいを入れてもらうやつ ***/
extern std::string g_result_path1, g_result_path2, g_result_path3;
/*** opencvの差分検出方法の選択のためのグローバル変数 ***/
extern int g_point;

//std::string error;
#define RESULT_PATH "/home/user/Qt/testresult.png"
#define ERRORcheck "error01"

///*** opencvの差分検出方法の選択のためのグローバル変数 ***/
//int g_point;

/*** カメラ入力のカウントを行うための変数 ***/
int g_count = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("間違い撲滅委員会〜打倒サイゼリ○〜"));

    /*** 1ページ目から始まるように ***/
    ui->stackedWidget->setCurrentIndex(0);

    /*** 背景色を薄い黄色にしている ***/
    centralWidget()->setStyleSheet("background:lemonchiffon;");

    /*** スタートボタンの調整 ***/
    ui->startButton->setFlat(true);
    ui->startButton->setStyleSheet("background-color:lemonchiffon;");

    /*** 各ボタンに対して背景の色が透けないようにしている ***/
    /*** 初芝担当箇所ボタン ***/
    ui->quitButton->setStyleSheet("background-color:gainsboro;");
    ui->fileSelectButton->setStyleSheet("background-color:gainsboro;");
    ui->cameraButton_1->setStyleSheet("background-color:gainsboro;");
    ui->returnButton_1->setStyleSheet("background-color:gainsboro;");
    ui->fileOpenButton_1->setStyleSheet("background-color:gainsboro;");
    ui->fileOpenButton_2->setStyleSheet("background-color:gainsboro;");
    ui->returnButton_2->setStyleSheet("background-color:gainsboro;");
    ui->cameraTakeButton_1->setStyleSheet("background-color:gainsboro;");
    /*** 廣瀬担当箇所ボタン ***/
    ui->startbutton->setStyleSheet("background-color:gainsboro;");
    ui->giveupbutton->setStyleSheet("background-color:gainsboro;");
    ui->clearbutton->setStyleSheet("background-color:gainsboro;");
    /*** 浅川担当箇所ボタン ***/
    ui->select_cheat->setStyleSheet("background-color:gainsboro;");
    ui->select_jiriki->setStyleSheet("background-color:gainsboro;");
    ui->start_return_Button->setStyleSheet("background-color:gainsboro;");
    ui->startReturnButton_2->setStyleSheet("background-color:gainsboro;");

    /*** BGM ***/
    /** 現在のファイルの絶対パスの取得 **/
    auto abs_path = QApplication::applicationDirPath();
    std::ostringstream music_path1;
    music_path1 << abs_path.toStdString() <<  "/../music/italy.mp3"; //ここ変えればBGM変えられる
    if(DEBUG){
        std::cout << music_path1.str() << std::endl;
    }
    QString qmusic_path1 = QString::fromStdString(music_path1.str());

    QMediaPlaylist *playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl::fromLocalFile(qmusic_path1));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    QMediaPlayer* background_music = new QMediaPlayer();
    background_music->setPlaylist(playlist);
    background_music->play();

    /*** 廣瀬担当箇所 ***/
    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(updateTime()));
//    QPixmap pix("../pic/software_logo_half3.png");//間違い撲滅委員会ロゴ
    QPixmap comp("../pic/comp.PNG");//クリア画面
    //スタート画面にロゴを表示
//    int w = ui->label->width();
//    int h = ui->label->height();
//    ui->label->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    //クリア画面を表示
    int w3 = ui->clearlabel->width();
    int h3 = ui->clearlabel->height();
    ui->clearlabel->setPixmap(comp.scaled(w3,h3,Qt::KeepAspectRatio));

    //間違いの残りの個数
    ui->allnumlabel->setText("全部で"+QString::number(num)+"こあるよ!");

    /*** 浅川担当箇所 ***/
}

/*** デストラクタ ***/
MainWindow::~MainWindow()
{
    getUsbCamT->Stop = true; //USBカメラ
    delete ui;
}

/** USBカメラ画像表示 **/
void MainWindow::onValueChangedCam()
{
    cv::Mat dst;
    getUsbCamT->getImage(&dst);
    qtImage = QImage((const unsigned char*)(dst.data), dst.cols, dst.rows, QImage::Format_RGB888);
    ui->cameraLabel_1->setPixmap(QPixmap::fromImage(qtImage));
}

/*** 画像処理後に次のページに移るためのコールバック関数 ***/
void MainWindow::moveNextPage()
{
    // QPixmap img1(../pic/test1.png");//image1
    QPixmap img1(QString::fromStdString(g_img_path1));
    // QPixmap img2("../pic/test2.png");//image2//
    QPixmap img2(QString::fromStdString(g_img_path2));
    //1つ目の画像を表示
    int w1 = ui->imagelabel1->width();
    int h1 = ui->imagelabel1->height();
    ui->imagelabel1->setPixmap(img1.scaled(w1,h1,Qt::KeepAspectRatio));
    //2つ目の画像を表示
    int w2 = ui->imagelabel2->width();
    int h2 = ui->imagelabel2->height();
    ui->imagelabel2->setPixmap(img2.scaled(w2,h2,Qt::KeepAspectRatio));
    //結果画面を表示
    QString Qresult_path = QString::fromStdString(g_result_path2); //受け取ったパスを変換
    QPixmap pix(Qresult_path);
    ui->result->setPixmap(pix);

    /** 次のページに行く **/
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++current_page);
    if(DEBUG) std::cout << "画像処理終わったよ" << std::endl;
}

/*** 画像処理時にエラーが出てエラー画面に移動するためのコールバック関数 ***/
void MainWindow::moveErrorPage()
{
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(current_page+3);
    if(DEBUG) std::cout << "画像処理エラーだよ" << std::endl;
}


/*** 終了ボタンを押したとき ***/
void MainWindow::on_quitButton_released()
{
    exit(0);
}

/*** スタート画面のボタンを押したとき ***/
void MainWindow::on_startButton_released()
{
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++current_page);
}

/*** ファイル選択ボタンを押したとき ***/
void MainWindow::on_fileSelectButton_released()
{
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(current_page+2);
    ui->debuglabel_2->setText("1枚目の画像を選択してください。");
    g_point = 1;
}

/*** 2ページ目(ファイルorカメラ選択)の戻るボタンを押したとき ***/
void MainWindow::on_returnButton_1_released()
{
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(--current_page);
}

/*** ファイル選択の1枚目のボタンを押したとき ***/
void MainWindow::on_fileOpenButton_1_released()
{
    QString selFilter, img_path1;
    img_path1 = QFileDialog::getOpenFileName(this, tr("1枚目の画像選択"), "", tr("Image(*.png *.jpg *.jpeg)"), &selFilter, QFileDialog::DontUseCustomDirectoryIcons); //opencvに渡すには共通の変数にするべきだね
    g_img_path1 = img_path1.toStdString();
    if(VIEWPATH){
        std::cout << "g_img_path1 is " << g_img_path1 << std::endl;
    }
    if(g_img_path1.empty()){
        ui->debuglabel_2->setText("1枚目の画像の取得に失敗しました。もう一度選択してください。");
    }else{
        ui->debuglabel_2->setText("2枚目の画像を選択してください。");
    }
}

/*** ファイル選択の2枚目のボタンを押したとき ***/
void MainWindow::on_fileOpenButton_2_released()
{
    QString img_path2;
    if(g_img_path1.empty()){
        ui->debuglabel_2->setText("1枚目の画像を先に選択してください。");
    }else{
        QString selFilter;
        img_path2 = QFileDialog::getOpenFileName(this, tr("2枚目の画像選択"), "", tr("Image(*.png *.jpg *.jpeg)"), &selFilter, QFileDialog::DontUseCustomDirectoryIcons); //opencvに渡すには共通の変数にするべきだね
        g_img_path2 = img_path2.toStdString();
        if(VIEWPATH){
            std::cout << "g_img_path2 is " << g_img_path2 << std::endl;
        }
        if(g_img_path2.empty()){
            ui->debuglabel_2->setText("2枚目の画像の取得に失敗しました。もう一度選択してください。");
        }else{
            ui->debuglabel_2->setText("");
            auto current_page = ui->stackedWidget->currentIndex();
            ui->stackedWidget->setCurrentIndex(++current_page);
            /*** 別スレッドで画像処理スタート ***/
           pointMatch = new pointMatching(this);
           connect(pointMatch, SIGNAL(nextPage()), this, SLOT(moveNextPage()));
           connect(pointMatch, SIGNAL(errorPage()), this, SLOT(moveErrorPage()));
           if(DEBUG){
               std::cout << "画像処理スタート" << std::endl;
           }
           pointMatch->start();
        }
    }

}

/*** ファイル画像を選択するときに戻るボタンを押したとき ***/
void MainWindow::on_returnButton_2_released()
{
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(current_page-2);
}

/*** 2ページ目(ファイルorカメラ選択)でカメラボタンを押したとき ***/
void MainWindow::on_cameraButton_1_released()
{
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++current_page);
    ui->debuglabel_3->setText("1枚目の画像を撮影してください。");
    g_point = 2;
    if(DEBUG){
        std::cout << "NextPage" << std::endl;
    }
    /**USBカメラ**/
    getUsbCamT = new getUsbCamera(this);
    if(getUsbCamT->initCam()){
        if(DEBUG) std::cout << "connect start!!!" << std::endl;
        connect(getUsbCamT, SIGNAL(valueChangedCam(void)), this, SLOT(onValueChangedCam(void)));
        if(DEBUG){
            std::cout << "Thread start!!!" << std::endl;
        }
        getUsbCamT->start();
    }else{
        QMessageBox msgBox(this);
        msgBox.setText(tr("カメラがオープンできませんでした。"));
        msgBox.setWindowTitle(tr("エラー"));
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        ui->debuglabel_1->setText("カメラ画像の取得に失敗しました。もう一度選択してください。");
        auto current_page = ui->stackedWidget->currentIndex();
        ui->stackedWidget->setCurrentIndex(--current_page);
    }
}

/*** カメラ画像が出て撮影ボタンを押したとき ***/
void MainWindow::on_cameraTakeButton_1_released()
{
    cv::Mat img;
    getUsbCamT->getImage(&img);
    cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
//    qtImage = QImage((const unsigned char*)(img.data), img.cols, img.rows, QImage::Format_RGB888);

//    auto abs_path = QApplication::applicationDirPath();
//    std::ostringstream camera_path1, camera_path2;
//    camera_path1 << abs_path.toStdString() <<  "/camera1_img.png";
//    camera_path2 << abs_path.toStdString() <<  "/camera2_img.png";
    g_count++;
    if(g_count == 1){
        g_img_path1 = "./camera1_img.png";
        cv::imwrite(g_img_path1, img);

        if(VIEWPATH){
            std::cout << "g_img_path1 is " << g_img_path1 << std::endl;
        }
        if(g_img_path1.empty()){
            ui->debuglabel_3->setText("もう一度1枚目の画像を撮影してください。");
            g_count--;
        }else{
            ui->debuglabel_3->setText("2枚目の画像を撮影してください。");
        }
    }else{
        g_img_path2 = "./camera2_img.png";
        cv::imwrite(g_img_path2, img);
//        cv::imwrite(camera_path2.str(), img);
//        g_img_path2 = camera_path2.str();
        if(g_img_path2.empty()){
            ui->debuglabel_3->setText("もう一度2枚目の画像を撮影してください。");
        }else{
            if(VIEWPATH){
                std::cout << "g_img_path2 is " << g_img_path2 << std::endl;
            }
            g_count = 0;
            ui->debuglabel_3->setText("");
            getUsbCamT->Stop = true;
            auto current_page = ui->stackedWidget->currentIndex();
            ui->stackedWidget->setCurrentIndex(current_page+2);
            /*** 別スレッドで画像処理スタート ***/
            pointMatch = new pointMatching(this);
            connect(pointMatch, &pointMatching::nextPage, this, &MainWindow::moveNextPage);
            connect(pointMatch, &pointMatching::nextPage, this, &MainWindow::moveErrorPage);
            if(DEBUG){
                std::cout << "画像処理スタート" << std::endl;
            }
            pointMatch->start();
        }
    }
}

/*** 廣瀬担当箇所 ***/
void MainWindow::on_startbutton_released()
{
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++current_page);
    time10 = 10000;
    timer->start(10);
}

void MainWindow::on_radioButton_clicked(bool checked)//timerstart/stop
{
//    if(checked){
//        timer->start(10);
//    }
//    else{
//        timer->stop();
//    }
}

void MainWindow::updateTime()
{
    if(time10>0){
        --time10;
        ui->timelabel->setText("残り"+QString::number(time10/100.0,'f',0)+"秒");
    }else {
        timer->stop();
        for(QLabel* label : imageLabels){//丸を消去
            label->deleteLater();
        }
        imageLabels.clear();
        ui->timelabel->setText("タイムアップ");//画面遷移にする
        sleep(3);
        auto current_page = ui->stackedWidget->currentIndex();//本当は結果表示画面に行かないといけない
        ui->stackedWidget->setCurrentIndex(current_page - 2);
    }
}


void MainWindow::on_giveupbutton_released()//ギブアップボタン
{
    for(QLabel* label : imageLabels){//丸を消す
        label->deleteLater();
    }
    imageLabels.clear();
    timer->stop();
    auto current_page = ui->stackedWidget->currentIndex();//本当は結果表示画面に行かないといけない
    ui->stackedWidget->setCurrentIndex(current_page - 2);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{

    QPoint p = ui->imagelabel2->mapFromGlobal(QCursor::pos());
    QPoint pos = this->mapFromGlobal(QCursor::pos());
    //qDebug() << "press:" << p;
    int x = p.x();
    int y = p.y();
    //QPixmap ans("../pic/sample_ans.png");//結果画像
//    QPixmap ans("../pic/Result.png");//結果画像
    QPixmap ans(QString::fromStdString(g_result_path2));
    ans = ans.scaled(ui->imagelabel2->width(),ui->imagelabel2->height(),Qt::KeepAspectRatio);//結果画像の大きさを調節・
    //ui->imagelabel2->setPixmap(ans.scaled(ui->imagelabel2->width(),ui->imagelabel2->height(),Qt::KeepAspectRatio));
    QImage ansimg = ans.toImage();
    QColor color = ansimg.pixelColor(x,y);
    qDebug() << ansimg.pixelColor(x,y);
    int red = color.red();

    if(red == 255){//正解だったとき
        num--;
        for(int i=-25;i<26;i++){
            for(int j=-25;j<26;j++){
                QRgb pixel = ansimg.pixel(x+i,y+j);
                pixel = qRgb(0,qGreen(pixel),qBlue(pixel));
                ansimg.setPixel(x+i,y+j,pixel);
            }
        }



        //QString savePath = "/home/user/Qt/start/sample_ans.png";
//        QString savePath = "/home/user/Qt/start/Result.png";
//        ansimg.save(savePath);
        ansimg.save(QString::fromStdString(g_result_path2));

        //正解した箇所に丸を表示
        QLabel* imageLabel = new QLabel(this);
        QPixmap cir("../pic/touka.png");
        imageLabel->setFixedSize(40,40);
        imageLabel->setPixmap(cir);
        imageLabel->move(pos.x()-20,pos.y()-20);
        imageLabel->show();
        imageLabels.append(imageLabel);//keep
        if(num == 0){//間違いをすべて見つけたとき
            for(QLabel* label : imageLabels){//○をすべて削除
                label->deleteLater();
            }
            imageLabels.clear();
            auto current_page = ui->stackedWidget->currentIndex();
            ui->stackedWidget->setCurrentIndex(++current_page);

        }
        ui->kosulabel->setText("あと"+QString::number(num)+"こ");
    }
    else{//不正解だったとき
        auto abs_path = QApplication::applicationDirPath();
        std::ostringstream buuFilePath;
        buuFilePath << abs_path.toStdString() <<  "/../music/buu.mp3"; //ここ変えればBGM変えられる
        if(DEBUG){
            std::cout << buuFilePath.str() << std::endl;
        }
        QString qbuuFilePath = QString::fromStdString(buuFilePath.str());

        QMediaPlayer* player = new QMediaPlayer;
        player->setMedia(QUrl::fromLocalFile(qbuuFilePath));
        player->play();

    }
    //qDebug() << "red" << red;
}

void MainWindow::on_clearbutton_released()
{
    // exit(0);
   auto current_page = ui->stackedWidget->currentIndex();
   ui->stackedWidget->setCurrentIndex(current_page=0);//ゲームクリア後スタートへ戻る
}

/*** 浅川担当箇所 ***/
void MainWindow::on_select_cheat_released()
{
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++current_page); //発表準備中の画面に飛んでから処理できるようにしたい
    //QTimer::singleShot(2000,this);
    //QThread::sleep(2); 遷移する前の画面で止まっちゃう

    //error = ERRORcheck; //エラー判断用のグローバル変数を作成し、エラーの場合はそこに何かしらの文字列を入れてもらうようにしたい（今回は"error01"）
//    QString Qerror = QString::fromStdString(error); //受け取った文字列を変換

//    if(Qerror.compare(ERRORcheck, Qt::CaseSensitive) == 0 ){
//        auto current_page = ui->stackedWidget->currentIndex();
//        ui->stackedWidget->setCurrentIndex(current_page++); //受け取った文字列が予め決めていた文字列と一致した場合、エラー画面に遷移
//    }
//    else{
        //結果を表示する前に、2秒ほど会議中の画面を入れたほうがいいかなと思ったので考えてます
        current_page = ui->stackedWidget->currentIndex();
        ui->stackedWidget->setCurrentIndex(current_page + 2); //結果表示画面に遷移
//        g_result_path3 = RESULT_PATH; //受け取ったパス(仮)を代入
//        QString Qresult_path = QString::fromStdString(g_result_path1); //受け取ったパスを変換
//        QPixmap pix(Qresult_path);
//        ui->result->setPixmap(pix);
//    }
}

void MainWindow::on_select_jiriki_released()
{
    auto current_page = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(current_page + 4);//自力モードへ
}

void MainWindow::on_start_return_Button_released()
{
    ui->stackedWidget->setCurrentIndex(0);//ゲームクリア後スタートへ戻る
}

void MainWindow::on_startReturnButton_2_released()
{
    ui->stackedWidget->setCurrentIndex(0);//スタートへ戻る
}
