//
// Created by xlz on 18-1-10.
//
#include <memory>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "pvaDetector.h"
#include <glog/logging.h>
#include <chrono>

#include "time.h"

#include "../src/myfUtils/FileOperator.h"
#include "../src/myfUtils/CropToPieces.h"
#include "../src/myfUtils/MatOperation.h"
#include "../src/myfUtils/NmsThreshFilterBoxes.h"



using namespace std;
using namespace cv;

void fileter2ndStageDets(std::vector<Detection>& s2_dets,int roiwidth,int roiheight,string stage1st_type){

    if(stage1st_type == "cross") {

        for (int ind = 0; ind < s2_dets.size(); ind++) {
            if (s2_dets[ind].getClass() == "type_none") {
                Rect rect = s2_dets[ind].getRect();
                int thresh_y = int(0.75*roiheight);
                if(rect.y>thresh_y){
                    s2_dets.erase(s2_dets.begin()+ind);
                    ind-=1;
                }
            }
        }

        // 第er级规则：剔除高宽比大于1.2的det
        for(int ind=0;ind<s2_dets.size();ind++){
            if(s2_dets[ind].getClass()=="type_none"){
                Rect rect = s2_dets[ind].getRect();
                double aspect_ratio = (double)rect.height/rect.width;
                if(aspect_ratio>1.0){
                    s2_dets.erase(s2_dets.begin()+ind);
                    ind-=1;
                }
            }
        }

    }else if(stage1st_type=="others"){

        // 第一级规则：剔除高宽比大于1.2的det
        for(int ind=0;ind<s2_dets.size();ind++){
            if(s2_dets[ind].getClass()=="type_none"){
                Rect rect = s2_dets[ind].getRect();
                double aspect_ratio = (double)rect.height/rect.width;
                if(aspect_ratio>1.0){
                    s2_dets.erase(s2_dets.begin()+ind);
                    ind-=1;
                }
            }
        }

        // 第二级规则：有一个good就剔除
        bool has_type_chaxiao_or_head=false;
        for(int ind=0;ind<s2_dets.size();ind++) {
            if(s2_dets[ind].getClass()=="type_head"||s2_dets[ind].getClass()=="type_chaxiao")has_type_chaxiao_or_head=true;
        }
        if(has_type_chaxiao_or_head){
            for(int ind=0;ind<s2_dets.size();ind++){
                if(s2_dets[ind].getClass()=="type_none"){
                    s2_dets.erase(s2_dets.begin()+ind);
                    ind-=1;
                }
            }
        }
    }

    int jjjj=1;
}

//#define POSITIVE
int main(int argc, char* argv[])
{
    google::InitGoogleLogging("mytest"); //初始化 glog
    google::InstallFailureSignalHandler();

    /**
     * a two stage pipeline for Kaikouxiao detection
     * step1: set input_dir
     * step2: set output_dir
     */

#ifdef POSITIVE
    char* inputdir = "/home/cuizhou/projects/KaiKouXiao/originaldata&&annotation/Positive/original";
//    char* inputdir = "/home/cuizhou/projects/KaiKouXiao/test_ws/problem";
    string outputdir = "/home/cuizhou/projects/KaiKouXiao/test_ws/test_pipeline_result_2round/posi";
#else
    //char* inputdir = "/home/cuizhou/projects/KaiKouXiao/originaldata&&annotation/Negative";
    char* inputdir = "/home/zhida/Documents/Data/DianWangKaiKouXiao-test/Negative";
    //string outputdir = "/home/cuizhou/projects/KaiKouXiao/test_ws/test_pipeline_result_2round/neg";
    string outputdir = "/home/zhida/Documents/Data/DianWangKaiKouXiao-test/test_ws/test_pipeline_result_2round/neg";
#endif

    //S1 PVANET Parameters
//    string s1_net_model = "../../models/kaikouxiaomodel_1ststage_1stround/test.caffemodel";
//    string s1_net_pt = "../../models/kaikouxiaomodel_1ststage_1stround/test.prototxt";
//    std::vector<std::string> s1_classes = { "__background__",
//                                             "simple","triangle","wings","cross","girder","others" };
    //string s1_net_model = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_1st_stage_2nd_round_150000_merge_svd.caffemodel";
    string s1_net_model = "/home/zhida/Documents/Code/cxx_pva/models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_1st_stage_2nd_round_150000_merge_svd.caffemodel";
    //string s1_net_pt = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_svd.prototxt";
    string s1_net_pt = "/home/zhida/Documents/Code/cxx_pva/models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_svd.prototxt";
    
    std::vector<std::string> s1_classes = { "__background__",
                                            "simple","triangle","wings","cross","girder","others" };
    //初始化
    PVADetector s1_detector;
    s1_detector.init(s1_net_pt, s1_net_model, s1_classes);
    s1_detector.setThresh(0.7, 0.3);
    s1_detector.setComputeMode("gpu", 0);


    //S2 PVANET Parameters
//    string s2_net_model = "../../models/kaikouxiaomodel_2ndstage_1stround/kaikouxiao_150000_merge_svd.caffemodel";
//    string s2_net_pt = "../../models/kaikouxiaomodel_2ndstage_1stround/kaikouxiao_svd.prototxt";
//    std::vector<std::string> s2_classes = { "__background__",
//                                            "type_chaxiao","type_none"}; // todo: 类别反了
    //string s2_net_model = "../../models/kaikouxiaomodel_2ndstage_2ndround/kaikouxiao_2ndstage_2round_100000_merge_svd.caffemodel";
    string s2_net_model = "/home/zhida/Documents/Code/cxx_pva/models/kaikouxiaomodel_2ndstage_2ndround/kaikouxiao_2ndstage_2round_100000_merge_svd.caffemodel";
    //string s2_net_pt = "../../models/kaikouxiaomodel_2ndstage_2ndround/kaikouxiao_svd.prototxt";
    string s2_net_pt = "/home/zhida/Documents/Code/cxx_pva/models/kaikouxiaomodel_2ndstage_2ndround/kaikouxiao_svd.prototxt";
    
    std::vector<std::string> s2_classes = { "__background__","type_none","type_chaxiao","type_head"}; // todo: 类别反了
    //初始化
    PVADetector s2_detector;
    s2_detector.init(s2_net_pt, s2_net_model, s2_classes);
    s2_detector.setThresh(0.7, 0.3);
    s2_detector.setComputeMode("gpu", 0);

    //初始化切片器
    CropToPieces crop_to_pieces(0,outputdir);

    //初始化nms过滤器
    NmsThreshFilterBoxes nms_boxes_filter(0);

    std::vector<std::string> filenames = readFileList(inputdir);
    for (auto filename:filenames) {
        cout<<filename<<endl;
        //计时
        clock_t start, finish;
        double duration;
        //开始
        start = clock();

        string imddir = inputdir;

        Mat srcImage = imread(imddir + "/" + filename);

        /* 切片 mat rows cols */

#ifdef POSITIVE
        vector<piece> pool_3_3 = crop_to_pieces.crop2pieces2(srcImage,3,3);
        //vector<piece> pool_1_3 = crop_to_pieces.crop2pieces2(srcImage,3,1);
        // 合并两个容器
        //pool_3_3.insert(pool_3_3.end(), pool_1_3.begin(), pool_1_3.end()); // 把v2加到v1末尾
#else
        vector<piece> pool_3_3 = crop_to_pieces.crop2pieces2(srcImage,1,1);
#endif
        // 申请kaikouxiao 的detection 结果 存储
        std::vector<Detection> s1_dets_final;
        std::vector<Detection> s2_dets_final;

        for(int ind_piece=0; ind_piece<pool_3_3.size();ind_piece++){

            /* Stage1 检测 , 检测类别包括  "simple","triangle","wings","cross","girder","others" */

            piece one_piece = pool_3_3[ind_piece];

            std::vector<Detection> s1_dets = s1_detector.detect(one_piece.piece_mat);

            vector<Detection> s1_dets_for_vis;
            for(int ind_s1_det=0;ind_s1_det<s1_dets.size();ind_s1_det++){
                Rect part_rect = s1_dets[ind_s1_det].getRect();
                int Trans_x = one_piece.left_top.x;
                int Trans_y = one_piece.left_top.y;
                Rect tmp_rect( part_rect.x+Trans_x,part_rect.y+Trans_y,part_rect.width,part_rect.height);
                Detection tmpdet = s1_dets[ind_s1_det];
                tmpdet.setRect(tmp_rect);
                s1_dets_for_vis.push_back(tmpdet);
            }
            s1_dets_final.insert(s1_dets_final.end(),s1_dets_for_vis.begin(),s1_dets_for_vis.end());

            cout<<"    s1 ... ... "<< s1_dets.size()<<" in total" <<endl;

            for(int ind_s1_det=0;ind_s1_det<s1_dets.size();ind_s1_det++){

                /* Stage2 检测 , 检测类别包括  "type_none","type_chaxiao" */

                if(s1_dets[ind_s1_det].getClass()=="girder")continue;

                Mat detMat;
                Rect part_rect = s1_dets[ind_s1_det].getRect();
                one_piece.piece_mat(part_rect).copyTo(detMat);
                std::vector<Detection> s2_dets = s2_detector.detect(detMat);
                cout<<"        s2 ..." <<endl;

                // 更具螺丝出现在部件上的位置来筛选螺丝
                fileter2ndStageDets(s2_dets,detMat.cols,detMat.rows,s1_dets[ind_s1_det].getClass());
                // 绘制可视化结果
                //s2_detector.drawBox(detMat,s2_dets);

                //在s2的检测结果上添加 切片+部件 位移
                for(int ind_s2_det=0;ind_s2_det<s2_dets.size();ind_s2_det++){
                    int Trans_x = one_piece.left_top.x + part_rect.x;
                    int Trans_y = one_piece.left_top.y + part_rect.y;
                    Rect s2_tmp_rect = s2_dets[ind_s2_det].getRect();
                    Rect tmp_rect(s2_tmp_rect.x+Trans_x,s2_tmp_rect.y+Trans_y,s2_tmp_rect.width,s2_tmp_rect.height);
                    s2_dets[ind_s2_det].setRect(tmp_rect);
                }

                //将s2检测结果添加到 s2_dets_final
                s2_dets_final.insert(s2_dets_final.end(),s2_dets.begin(),s2_dets.end());
            }
        }

        nms_boxes_filter.filter(s2_dets_final);
        s1_detector.drawBox(srcImage,s2_dets_final,"type_none");

        nms_boxes_filter.filter(s1_dets_final);
        s1_detector.drawBox(srcImage,s1_dets_final);

        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;

        imwrite(outputdir+"/"+filename,srcImage);
        cout<<"wrote ------------- : "<<filename << " ------ cost time: " << duration << "s" <<endl;
    }

    return 0;
}