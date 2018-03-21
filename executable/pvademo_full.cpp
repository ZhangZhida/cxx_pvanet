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

#include "../src/myfUtils/MatOperation.h"


using namespace std;
using namespace cv;


int main(int argc, char* argv[]) {

    string inputPath = "/home/zhida/Documents/Data/DianWangKaiKouXiao-test/Negative/184.jpg";
    string outputPath = "/home/zhida/Documents/Data/DianWangKaiKouXiao-test/test_ws";

    string net_model_1 = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_1st_stage_2nd_round_150000_merge_svd.caffemodel";
    string net_pt_1 = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_svd.prototxt";
    std::vector<std::string> s1_classes = { "__background__",
                                            "simple","triangle","wings","cross","girder","others" };

    string net_model_2 = "../../models/kaikouxiaomodel_2ndstage_2ndround/kaikouxiao_2ndstage_2round_100000_merge_svd.caffemodel";
    string net_pt_2 =  "../../models/kaikouxiaomodel_2ndstage_2ndround/kaikouxiao_svd.prototxt";
    std::vector<std::string> s2_classes = { "__background__",
                                            "type_none","type_chaxiao","type_head"};

    
    // ### ---- first stage ---- ###


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

    // ## crop
    //保存det roi.
    vector<Mat> matpool;
    for(int ind_det=0;ind_det<dets.size();ind_det++){
        Mat detMat;
        src_image(dets[ind_det].getRect()).copyTo(detMat);
        matpool.push_back(detMat);
    }

    vector<string> roi_filepaths = writeMatPool(matpool, outputPath+"/roi_images");

    for(int i=0; i<roi_filepaths.size(); i++) {

        string roi_filepath = roi_filepaths[i];
        Mat roi_src_image = imread(roi_filepath);

        string roi_window_name = "roi" + i;
        //cv::namedWindow(roi_window_name, CV_WINDOW_AUTOSIZE);
        //cv::imshow(roi_window_name, roi_src_image);
    }


    // ### ---- second stage ---- ###
    
    PVADetector detector_2;
    detector_2.init(net_pt_2, net_model_2, s2_classes);

    detector_2.setThresh(0.7, 0.3);
    detector_2.setComputeMode("gpu", 0);

    for (int i=0; i<roi_filepaths.size(); i++) {

        string roi_filepath = roi_filepaths[i];
        Mat roi_image = imread(roi_filepath);

        vector<Detection> det2s = detector_2.detect(roi_image);
        detector_2.drawBox(roi_image, det2s);

        string roi_window_name = "roi" + i;
        cv::namedWindow(roi_window_name, CV_WINDOW_AUTOSIZE);
        cv::imshow(roi_window_name, roi_image);

        // ## crop
        //保存det roi_chaxiao.
        vector<Mat> matpool_roi;
        for(int ind_det=0;ind_det<det2s.size();ind_det++){
            Mat detMat_chaxiao;
            roi_image(det2s[ind_det].getRect()).copyTo(detMat_chaxiao);
            matpool_roi.push_back(detMat_chaxiao);
        }

        vector<string> roi_chaxiao_filepaths = writeMatPool(matpool_roi, outputPath+"/chaxiao_images");

        for(int i=0; i<roi_chaxiao_filepaths.size(); i++) {

            string roi_chaxiao_filepath = roi_chaxiao_filepaths[i];
            Mat roi_chaxiao_src_image = imread(roi_chaxiao_filepath);

            string roi_chaxiao_window_name = "chaxiao" + i;
            cv::namedWindow(roi_chaxiao_window_name, CV_WINDOW_AUTOSIZE);
            cv::imshow(roi_chaxiao_window_name, roi_chaxiao_src_image);
        }

    }



    cv::waitKey(0);


}