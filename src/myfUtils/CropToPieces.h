//
// Created by cuizhou on 18-3-12.
//

#ifndef CXX_PVA_INTERFACE_CROPTOPIECES_H
#define CXX_PVA_INTERFACE_CROPTOPIECES_H


#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;


struct piece{
    Mat piece_mat;
    Point left_top;
};
class CropToPieces
{
public:
    CropToPieces(int,string);

    vector<piece> crop2pieces(Mat& srcImages,int rows,int cols);
    vector<piece> crop2pieces2(Mat& srcImage,int rows,int cols);

    void setImageName(string imagename);

    //void writeresults(vector<piece>& pool);
    void writeresults(vector<piece>& pool);

    //获取每一个切片的左上角点坐标
//    vector<Point> getLeftTopCoors(int ind);


    void update(){
        _imgcount++;
    }

private:
    string _imagename;
    string _cuttype;
    string _savedir;
    int _imgcount;
};



#endif //CXX_PVA_INTERFACE_CROPTOPIECES_H
