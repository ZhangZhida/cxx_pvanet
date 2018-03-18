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
#include "../src/myfUtils/FileOperator.h"
#include "../src/myfUtils/CropToPieces.h"
#include "../src/myfUtils/MatOperation.h"

using namespace std;
using namespace cv;

//#define save_qiepian  //切片->保存切片
//#define save_qiepian_visresult  //切片->检测->保存切片可视化图片
#define save_qiepian_roimat  //切片->检测->保存检测框内的mat

#ifdef save_qiepian
int main(int argc, char* argv[])
{
    /**
     * 切片->保存切片
     * step1: set input_dir
     * step2: set output_dir
     */

    char* inputdir = "/home/cuizhou/projects/KaiKouXiao/originaldata&&annotation/Positive/original";
    string outputdir = "/home/cuizhou/projects/KaiKouXiao/test_ws/stage1_result/qiepian_positive_high_resolution_cxx";
    //初始化切片器
    CropToPieces crop_to_pieces(0,outputdir);

    std::vector<std::string> filenames = readFileList(inputdir);
    for (auto filename:filenames) {
        string imddir = inputdir;

        Mat srcImage = imread(imddir + "/" + filename);

        /* 切片 mat rows cols */
        vector<piece> pool_3_3 = crop_to_pieces.crop2pieces2(srcImage,3,3);
        // 保存切片图片
        crop_to_pieces.writeresults(pool_3_3);

        vector<piece> pool_1_3 = crop_to_pieces.crop2pieces2(srcImage,3,1);
        // 保存切片图片
        crop_to_pieces.writeresults(pool_1_3);

        //处理完一张图片之后更新
        crop_to_pieces.update();
    }

    return 0;
}
#endif
#ifdef save_qiepian_visresult
int main(int argc, char* argv[])
{
    /**
     * 切片-> 保存切片可视化图片
     * step1: set input_dir
     * step2: set output_dir
     */

    char* inputdir = "/home/cuizhou/projects/KaiKouXiao/originaldata&&annotation/Positive/original";
    string outputdir = "/home/cuizhou/projects/KaiKouXiao/test_ws/stage1_result/compare/c";

    //PVANET Parameters
//    string net_model = "../../models/kaikouxiaoStage1/test.caffemodel";
//    string net_pt = "../../models/kaikouxiaoStage1/test.prototxt";
//    std::vector<std::string> classes = { "__background__",
//                                             "simple","triangle","wings","cross","girder","others" };

    string s1_net_model = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_1st_stage_2nd_round_150000_merge_svd.caffemodel";
    string s1_net_pt = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_svd.prototxt";
    std::vector<std::string> s1_classes = { "__background__",
                                            "simple","triangle","wings","cross","girder","others" };
    //初始化
    PVADetector detector;
    detector.init(s1_net_pt, s1_net_model, s1_classes);
    detector.setThresh(0.7, 0.3);
    detector.setComputeMode("gpu", 0);

    //初始化切片器
    CropToPieces crop_to_pieces(0,outputdir);

    std::vector<std::string> filenames = readFileList(inputdir);
    for (auto filename:filenames) {
        string imddir = inputdir;

        Mat srcImage = imread(imddir + "/" + filename);


        /* 切片 mat rows cols */
        vector<Mat> pool_3_3 = crop_to_pieces.crop2pieces2(srcImage,3,3);
        // 保存切片图片
        //crop_to_pieces.writeresults(pool_3_3);

        vector<Mat> pool_1_3 = crop_to_pieces.crop2pieces2(srcImage,3,1);
        // 保存切片图片
//        crop_to_pieces.writeresults(pool_1_3);

        crop_to_pieces.update();


        /* 合并另个容器 */
        pool_3_3.insert(pool_3_3.end(), pool_1_3.begin(), pool_1_3.end()); // 把v2加到v1末尾

        /* 检测 */
        for(int idx_pool=0; idx_pool<pool_3_3.size();idx_pool++){
            Mat piece = pool_3_3[idx_pool];

            std::vector<Detection> dets = detector.detect(piece);
            detector.drawBox(piece, dets);

//            cv::imshow("piece", piece);
//            cv::waitKey(0);
            stringstream oo;
            oo<<outputdir<<"/"<<filename.substr(0,filename.length()-4)<<"_"<<idx_pool<<".jpg";
            imwrite(oo.str(),piece);
            cout<<"wrote: "<<filename<<endl;
        }

    }

    return 0;
}
#endif

#ifdef save_qiepian_roimat
int main(int argc, char* argv[])
{
    /**
     * 切片->检测->保存检测框内的mat
     * step1: set input_dir
     * step2: set output_dir
     */

    char* inputdir = "/home/cuizhou/projects/KaiKouXiao/originaldata&&annotation/Positive/original";
//    char* inputdir = "/home/cuizhou/projects/KaiKouXiao/originaldata&&annotation/Positive/original";
    string outputdir = "/home/cuizhou/projects/KaiKouXiao/test_ws/hardnegative_trainingdata_for_2ndround";

    //PVANET Parameters
//    string net_model = "../../models/kaikouxiaoStage1/test.caffemodel";
//    string net_pt = "../../models/kaikouxiaoStage1/test.prototxt";
//    std::vector<std::string> classes = { "__background__",
//                                             "simple","triangle","wings","cross","girder","others" };
    string s1_net_model = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_1st_stage_2nd_round_150000_merge_svd.caffemodel";
    string s1_net_pt = "../../models/kaikouxiaomodel_1ststage_2ndround/kaikouxiao_svd.prototxt";
    std::vector<std::string> s1_classes = { "__background__",
                                            "simple","triangle","wings","cross","girder","others" };
    //初始化
    PVADetector detector;
    detector.init(s1_net_pt, s1_net_model, s1_classes);
    detector.setThresh(0.7, 0.3);
    detector.setComputeMode("gpu", 0);


    //初始化切片器
    CropToPieces crop_to_pieces(0,outputdir);

    std::vector<std::string> filenames = readFileList(inputdir);
    for (auto filename:filenames) {
        string imddir = inputdir;

        Mat srcImage = imread(imddir + "/" + filename);

        /* 切片 mat rows cols */
        vector<piece> pool_3_3 = crop_to_pieces.crop2pieces2(srcImage,3,3);
        // 保存切片图片
        //crop_to_pieces.writeresults(pool_3_3);

        vector<piece> pool_1_3 = crop_to_pieces.crop2pieces2(srcImage,3,1);
        // 保存切片图片
//        crop_to_pieces.writeresults(pool_1_3);

        crop_to_pieces.update();


        /* 合并另个容器 */
        pool_3_3.insert(pool_3_3.end(), pool_1_3.begin(), pool_1_3.end()); // 把v2加到v1末尾

        /* 检测 */
        for(int ind_piece=0; ind_piece<pool_3_3.size();ind_piece++){
            Mat piece_mat = pool_3_3[ind_piece].piece_mat;

            std::vector<Detection> dets = detector.detect(piece_mat);

            //保存det roi.
            vector<Mat> matpool;
            for(int ind_det=0;ind_det<dets.size();ind_det++){
                Mat detMat;
                piece_mat(dets[ind_det].getRect()).copyTo(detMat);
                matpool.push_back(detMat);
            }

            writeMatPool(matpool,ind_piece,filename, outputdir);
            cout<<"wrote: "<<filename<<endl;
        }

    }

    return 0;
}
#endif

