#include "pointmatching.h"

#define DEBUG 1

/*** ファイル画像のパスを入れるためのグローバル変数 ***/
std::string g_img_path1, g_img_path2;
/*** 結果表示画面の画像ファイルパス入れてもらうやつ ***/
std::string g_result_path1, g_result_path2;
/*** opencvの差分検出方法の選択のためのグローバル変数 ***/
int g_point;


/*** コンストラクタ ***/
pointMatching::pointMatching(QObject *parent, bool b) : QThread(parent), Stop(b)
{

}

/*** デストラクタ ***/
pointMatching::~pointMatching()
{
    Stop = true;
}

/*** mainwindow.cppから呼び出されるやつ ***/
void pointMatching::run(void)
{
  if(DEBUG){
      std::cout << "img_path1:" << g_img_path1 << std::endl;
      std::cout << "img_path2:" << g_img_path2 << std::endl;
  }
  cv::Mat image1 = cv::imread(g_img_path1);
  cv::Mat image2 = cv::imread(g_img_path2);

  emit value_change(10);
  

  /*** カメラの画像を長方形に補正する ***/
  int num = g_point;
  if(num == 2){
    cropping(image1, image1);
    cropping(image2, image2);
    if(DEBUG) std::cout << "カメラ入力の補正終わったよ" << std::endl;
  }else{
      sleep(1);
  }

  emit value_change(30);
  sleep(1);
  emit value_change(40);
  sleep(1);
  emit value_change(50);
  sleep(1);
  emit value_change(60);
  
  //特徴点マッチング
  int end_match = feature_matching(image1, image2, image1, image2);
  if(DEBUG) std::cout << "特徴点マッチング終わったよ" << std::endl;

  emit value_change(90);
  sleep(1);
  
  if(end_match == 0){ //特徴点マッチングで類似度高い
      //差分検出＆補正をコマンドライン引数から選択->GUIのファイルorカメラでできるようにした
      if(DEBUG) std::cout << "g_point = " << num << std::endl;

      if(num == 1){
        if(DEBUG) std::cout << "差分検出1" << std::endl;
        absdiff1(image1, image2); //差分検出方法①(ファイル向け)
      }
      else if(num == 2){
        if(DEBUG) std::cout << "差分検出2" << std::endl;
        absdiff2(image1, image2); //差分検出方法②(カメラ向け)
      }

        /*** 結果画像のパスをグローバル変数に格納 ***/
//        std::string g_result_path1 = "./redImage.png";
//        std::string g_result_path2 = "./Result.png";
        g_result_path1 = "./redImage.png";
        g_result_path2 = "./Result.png";

        if(DEBUG){
            std::cout << "result_path1:" << g_result_path1 << std::endl;
            std::cout << "result_path2:" << g_result_path2 << std::endl;
        }

        emit value_change(100);
        sleep(1);

        /*** 終わったらシグナルを発信して次の画面に行く ***/
        emit nextPage();

  }else{
        if(DEBUG) std::cout << "エラー画面に移動" << std::endl;
        emit errorPage();
  }
}

void pointMatching::cropping(const cv::Mat &src, cv::Mat &dst){
  const static int THRESH_VALUE = 200; // 二値化するときの閾値
  cv::Mat gray, ret, ret2;

  /* 輪郭抽出の下準備(二値化とオープニング/クロージング) */
  cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
  cv::adaptiveThreshold(gray, ret, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 11, 5);
  cv::dilate(ret, ret, cv::Mat(), cv::Point(-1,-1), 1);//ココらへんを変える
  cv::erode(ret, ret, cv::Mat(), cv::Point(-1,-1), 1);//ココらへんを変える

  /* 最も外側の輪郭を抽出 */
  std::vector<std::vector<cv::Point>> contours; // 抽出時点の頂点
  cv::findContours(ret, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
  cv::cvtColor(ret, ret, cv::COLOR_GRAY2BGR);
  src.convertTo(ret2, CV_32F, 1.0/255);
  /* 輪郭を図形に近似 */
  std::vector<cv::Point2f> approx; // 輪郭の頂点
  for(auto contour = contours.begin(); contour != contours.end(); contour++){
    double epsilon = 0.08 * cv::arcLength(*contour, true);
    cv::approxPolyDP(cv::Mat(*contour), approx, epsilon, true);
    double area = cv::contourArea(approx);
    if(area < src.rows * src.cols / 2){
      dst = src;
      continue;
    }
    //cv::polylines(ret, approx, true, cv::Scalar(0, 0, 255), 2);
    //cv::drawContours(ret, *contour, -1, cv::Scalar(0, 0, 255), cv::LINE_AA);
  }
  /* 四角形の頂点が検出できていない場合は終了 */
  if(approx.size() != 4){
    dst = src;
    std::cout << "四角形の頂点の検出に失敗しました。検出できた頂点数は" << approx.size() << "個です。" << std::endl;
    return;
  }
  /* 透視変換 */
  std::vector<cv::Point2f> src_vertex = {approx[0], approx[1], approx[2], approx[3]};
  int dst_width  = std::hypot(approx[3].x - approx[0].x, approx[3].y - approx[0].y);
  int dst_height = std::hypot(approx[1].x - approx[0].x, approx[1].y - approx[0].y);
  std::vector<cv::Point2f> dst_vertex = {{0,0}, {0,(float)dst_height}, {(float)dst_width,(float)dst_height}, {(float)dst_width,0}};
  dst = cv::Mat::zeros(cv::Size(dst_width, dst_height), CV_32F);
  cv::Mat H = cv::getPerspectiveTransform(src_vertex, dst_vertex);
  std::cout << "check" << std::endl;
  cv::warpPerspective(src, dst, H, dst.size());
}

int pointMatching::feature_matching(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst1, cv::Mat &dst2)
{
  std::vector<cv::KeyPoint> key1, key2; // 特徴点を格納
  cv::Mat des1, des2; // 特徴量記述の計算

  /* 2画像を比較して2辺とも小さい場合はsrc2に合わせて補正できるように */
    if(src1.cols <= src2.cols && src1.rows <= src2.rows){
      dst1 = src2;
      dst2 = src1;
    }else{
      dst1 = src1;
      dst2 = src2;
    }

  /* 比較のために複数手法を記述 必要に応じてコメントアウト*/
  /* 特徴点検出*/
  /* AKAZE */
  cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();
  akaze->detectAndCompute(dst1, cv::noArray(), key1, des1);
  akaze->detectAndCompute(dst2, cv::noArray(), key2, des2);
  /* ORB */
  // cv::Ptr<cv::ORB> orb = cv::ORB::create();
  // orb->detectAndCompute(dst1, cv::noArray(), key1, des1);
  // orb->detectAndCompute(dst2, cv::noArray(), key2, des2);

  //std::cout << des1 << std::endl;

  /* 特徴点マッチングアルゴリズム */
  cv::Ptr<cv::DescriptorMatcher> matcher;
  std::vector<cv::DMatch> match;
  /* 総当たり */
  // matcher = cv::DescriptorMatcher::create("BruteForce");
  // matcher->match(des1, des2, match);
  /* FLANN */
  matcher = cv::DescriptorMatcher::create("FlannBased");
  std::vector<std::vector<cv::DMatch>> knn_matches;
  if(des1.type() != CV_32F) {
    des1.convertTo(des1, CV_32F);
  }
  if(des2.type() != CV_32F) {
    des2.convertTo(des2, CV_32F);
  }
  matcher->knnMatch(des1, des2, knn_matches, 2);
  /* ratio test */
  const float RATIO_THRESH = 0.7f;
  for(int i = 0; i < knn_matches.size(); i++){
    if(knn_matches[i][0].distance < RATIO_THRESH * knn_matches[i][1].distance){
      match.push_back(knn_matches[i][0]);
    }
  }

  /* 類似度計算(距離による実装、0に近い値ほど画像が類似) */
  float sim = 0;
  const float THRESHOLD = 300; // 類似度の閾値(仮)
  for(int i = 0; i < match.size(); i++){
    cv::DMatch dis = match[i];
    sim += dis.distance;
  }
  sim /= match.size();
  if(DEBUG) std::cout << "類似度: " << sim << std::endl;


  /* 画像の類似度が低すぎる場合は終了 */
  if( sim > THRESHOLD){
    std::cerr << "画像が違いすぎます" << std::endl;
    return -1;
  }

  /* 特徴量距離の小さい順にソートし、不要な点を削除 */
  for(int i = 0; i < match.size(); i++){
    double min = match[i].distance;
    int n = i;
    for(int j = i + 1; j < match.size(); j++){
      if(min > match[j].distance){
        n = j;
        min = match[j].distance;
      }
    }
    std::swap(match[i], match[n]);
  }
  if(match.size() > 50){
    match.erase(match.begin() + 50, match.end());
  }else{
    match.erase(match.begin() + match.size(), match.end());
  }

  //cv::drawMatches(src1, key1, src2, key2, match, dst);

  /* src1をsrc2に合わせる形で射影変換して補正 */
  std::vector<cv::Vec2f> get_pt1(match.size()), get_pt2(match.size()); // 使用する特徴点
  /* 対応する特徴点の座標を取得・格納*/
  for(int i = 0; i < match.size(); i++){
    get_pt1[i][0] = key1[match[i].queryIdx].pt.x;
    get_pt1[i][1] = key1[match[i].queryIdx].pt.y;
    get_pt2[i][0] = key2[match[i].trainIdx].pt.x;
    get_pt2[i][1] = key2[match[i].trainIdx].pt.y;
  }

  /* ホモグラフィ行列推定 */
  cv::Mat H = cv::findHomography(get_pt1, get_pt2, cv::RANSAC); 
  /* src1を変形 */
  cv::warpPerspective(dst1, dst1, H, dst2.size());

  return 0;
}

//差分検出方法①...しっかり検出できるが、少しでもずれると検出できない(カメラ向きでない)
void pointMatching::absdiff1(const cv::Mat &src, const cv::Mat &dst)
{
  // 読み込みの確認
  if (src.empty() || dst.empty())
  {
    std::cout << "読み込みに失敗しました" << std::endl;
    return;
  }

  // 画像のリサイズ
  cv::resize(dst, dst, src.size());

  // 画像の差分を計算
  cv::Mat diffImage;
  cv::absdiff(src, dst, diffImage);
  if(DEBUG) std::cout << "差分計算終了" << std::endl;

  // 差分画像をグレースケールに変換
  cv::cvtColor(diffImage, diffImage, cv::COLOR_BGR2GRAY);

  // ブラーでノイズ除去
  cv::medianBlur(diffImage, diffImage, 3);
  
  // 差分画像を2値化
  cv::threshold(diffImage, diffImage, 30, 255, cv::THRESH_BINARY);

  // 差分のある画素に赤色を付ける
  cv::Mat resultImage = src.clone();
  resultImage.setTo(cv::Scalar(0, 0, 255), diffImage);
  cv::Scalar lowerRed = cv::Scalar(0, 0, 255);
  cv::Scalar upperRed = cv::Scalar(0, 0, 255);
  if(DEBUG) std::cout << "赤色つける" << std::endl;

  // 画像から赤色部分を抽出
  cv::Mat redMask;
  cv::inRange(resultImage, lowerRed, upperRed, redMask);

  cv::Mat redImage;
  resultImage.copyTo(redImage, redMask);

  cv::imwrite("redImage.png", redImage);
  if(DEBUG) std::cout << "赤色抽出" << std::endl;

  // 画像の暗くする度合いを設定
  double alpha = 0.3; // 0.0から1.0の範囲で設定（1.0で元の明るさ、0.0で完全に暗くなる）

  // 画像を暗くする
  cv::Mat darkenedImage = src * alpha;
  if(DEBUG) std::cout << "画像暗くした" << std::endl;

  // redMaskと暗くした画像を重ねる
  cv::Mat overlaidImage;
  cv::addWeighted(redImage, 1.0, darkenedImage, 1.0, 0.0, overlaidImage);
  cv::resize(overlaidImage, overlaidImage, cv::Size(), 500.0/overlaidImage.cols ,500.0/overlaidImage.cols);
  if(DEBUG) cv::imshow("Resuit", overlaidImage);
//  cv::waitKey(0);
  cv::imwrite("Result.png", overlaidImage);
  if(DEBUG) std::cout << "暗くした画像と重ねた" << std::endl;
}

//差分検出方法②...変形に対して強いが、検出できない間違いも多い(カメラ向き)
void pointMatching::absdiff2(const cv::Mat &src, const cv::Mat &dst)
{
  // 読み込みの確認
  if (src.empty() || dst.empty())
  {
    std::cout << "読み込みに失敗しました" << std::endl;
    return;
  }
  // 画像のリサイズ
  cv::resize(dst, dst, src.size());
  cv::Mat WarpedSrcMat, LmatFloat, Rsrc;
  WarpedSrcMat.convertTo(LmatFloat, CV_16SC3);
  Rsrc.convertTo(Rsrc, CV_16SC3);
  std::vector<cv::Mat> planes1;
  std::vector<cv::Mat> planes2;
  cv::Mat diff0;
  cv::Mat diff1;
  cv::Mat diff2;

    // 3つのチャネルB, G, Rに分離 (OpenCVではデフォルトでB, G, Rの順)
    cv::split(src, planes1);
  cv::split(dst, planes2);

  ///分割したチャンネルごとに差分を出す
  cv::absdiff(planes1[0], planes2[0], diff0);
  cv::absdiff(planes1[1], planes2[1], diff1);
  cv::absdiff(planes1[2], planes2[2], diff2);

  // ブラーでノイズ除去
  cv::medianBlur(diff0, diff0, 3);
  cv::medianBlur(diff1, diff1, 3);
  cv::medianBlur(diff2, diff2, 3);
  cv::Mat wiseMat;
  cv::bitwise_or(diff0, diff1, wiseMat);
  cv::bitwise_or(wiseMat, diff2, wiseMat);

  //オープニング処理でノイズ緩和
  cv::Mat openingMat;
  cv::morphologyEx(wiseMat, openingMat, 0, openingMat);

  // スレッショルドで差分をきれいにくっきりと
  cv::Mat dilationMat;
  cv::dilate(openingMat, dilationMat, dilationMat);
  cv::threshold(dilationMat, dilationMat, 100, 255, cv::THRESH_BINARY);
  cv::medianBlur(dilationMat, dilationMat, 3);

  // dilationMatはグレースケールなので合成先のMatと同じ色空間に変換する
  cv::Mat dilationScaleMat;
  cv::Mat dilationColorMat;
  cv::convertScaleAbs(dilationMat, dilationScaleMat);
  cv::cvtColor(dilationScaleMat, dilationColorMat, cv::COLOR_GRAY2RGB);

 // 差分のある画素に赤色を付ける
  cv::Mat resultImage = src.clone();
  resultImage.setTo(cv::Scalar(0, 0, 255), dilationColorMat);
  cv::Scalar lowerRed = cv::Scalar(0, 0, 255);
  cv::Scalar upperRed = cv::Scalar(0, 0, 255);

  // 画像から赤色部分を抽出
  cv::Mat redMask;
  cv::inRange(resultImage, lowerRed, upperRed, redMask);
  cv::Mat redImage;
  resultImage.copyTo(redImage, redMask);
  cv::imwrite("redImage.png", redImage);

  // 画像の暗くする度合いを設定
  double alpha = 0.3; // 0.0から1.0の範囲で設定（1.0で元の明るさ、0.0で完全に暗くなる）

  // 画像を暗くする
  cv::Mat darkenedImage = src * alpha;

  // redMaskと暗くした画像を重ねる
  cv::Mat overlaidImage;
  cv::addWeighted(redImage, 1.0, darkenedImage, 1.0, 0.0, overlaidImage);
  cv::resize(overlaidImage, overlaidImage, cv::Size(), 500.0/overlaidImage.cols ,500.0/overlaidImage.cols);
  if(DEBUG) cv::imshow("Resuit", overlaidImage);
  //cv::waitKey(0);
  cv::imwrite("Result.png", overlaidImage);
}
