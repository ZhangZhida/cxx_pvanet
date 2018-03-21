//
// Created by cuizhou on 18-2-24.
//

#ifndef MSER_MYMATOPERATION_H
#define MSER_MYMATOPERATION_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <sstream>
using namespace cv;
using namespace std;

/**
 * 将一个mat vector组成一个可视化矩阵, 输入是灰度mat
 * @param matpool
 * @return
 */
Mat composeGrayVisMat(vector<Mat> matpool);

/**
 * 将一个mat vector保存成图片, 给出 文件名标志, 和原图片名称
 * @param matpool
 * @param Sig 文件名标志
 * @param filename 原图片名称
 * @param output_dir 保存目标目录
 */
vector<string> writeMatPool(vector<Mat> matpool,string Sig,string filename,string output_dir);
vector<string> writeMatPool(vector<Mat> matpool,int Sig,string filename,string output_dir);
vector<string> writeMatPool(vector<Mat> matpool,int Sig,string output_dir);
vector<string> writeMatPool(vector<Mat> matpool,string output_dir);

#endif //MSER_MYMATOPERATION_H
