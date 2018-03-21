//
// Created by ZhangZhida on 18-3-21.
//

#include <memory>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "pvaDetector.h"
#include <glog/logging.h>
#include <chrono>


using namespace std;
using namespace cv;


int main(int argc, char* argv[]) {

    string inputPath = "/home/zhida/Documents/Data/DianWangKaiKouXiao-test/9.JPGresize.jpg";

    string net_model_1 = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_1st_stage_2nd_round_150000_merge_svd.caffemodel";
    string net_pt_1 = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_svd.prototxt";
    std::vector<std::string> s1_classes = { "__background__",
                                            "simple","triangle","wings","cross","girder","others" };

    string net_model_2 = "../../models/kaikouxiaomodel_2ndstage_2ndround/kaikouxiao_2ndstage_2round_100000_merge_svd.caffemodel";
    string net_pt_2 =  "../../models/kaikouxiaomodel_2ndstage_2ndround/kaikouxiao_svd.prototxt";
    std::vector<std::string> s2_classes = { "__background__",
                                            "type_none","type_chaxiao","type_head"};


    // ## initialize
    PVADetector detector_1;
    detector_1.init(net_pt_1, net_model_1, s1_classes);

    detector_1.setThresh(0.7, 0.3);
    detector_1.setComputeMode("gpu", 0);

    // ## image
    string img_path = inputPath;
    cv::Mat src_image = cv::imread(img_path);

    // ## detect
    std::vector<Detection> dets = detector_1.detect(src_image);
    detector_1.drawBox(src_image, dets);

    // ## show
    cv::namedWindow("stage1", CV_WINDOW_AUTOSIZE);
    cv::imshow("stage1", src_image);

    cv::waitKey(0);


}