//
// Created by cuizhou on 18-3-12.
//

#include "CropToPieces.h"

CropToPieces::CropToPieces(int idx,string savedir) {
    _imgcount = idx;
    _savedir = savedir;
}

void inline getSafeBoundery(int start, int end,int& x){
    if(x<start)x=start;
    if(x>end)x=end;
}
vector<piece> CropToPieces::crop2pieces(Mat& srcImage,int rows,int cols){

    /**
     * params：
     * @rows x方向条数
     * @cols y方向条数
     *
     * 1. 先计算 条宽
     * 2. +重叠区域的宽度
     * 3. 计算边界范围
     * 4. 生成切片
     */

    vector<piece> pool;

    stringstream ouou;
    ouou<<"TPR"<<rows<<"TPC"<<cols;
    _cuttype = ouou.str();

    int piece_width = srcImage.cols/cols;
    int piece_height = srcImage.rows/rows;

    int extrawidth = piece_width/2;
    int extraheight = piece_height/2;

    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){

            piece temppiece;

            int startx = col * piece_width;
            int endx = (col+1) * piece_width;
            int starty = row * piece_height;
            int endy = (row+1) * piece_height;

            startx-=extrawidth;
            endx+=extrawidth;
            starty-=extraheight;
            endy+=extraheight;

            getSafeBoundery(0,srcImage.cols-1,startx);
            getSafeBoundery(0,srcImage.cols-1,endx);
            getSafeBoundery(0,srcImage.rows-1,starty);
            getSafeBoundery(0,srcImage.rows-1,endy);

            Mat temp_mat;
            srcImage(Rect(startx,starty,endx-startx,endy-starty)).copyTo(temp_mat);

//            imshow("hahsdhfa",piece_temp);
//            waitKey(0);
            temppiece.piece_mat = temp_mat;
            temppiece.left_top = Point(startx,starty);

            pool.push_back(temppiece);
        }
    }

    return pool;

}

vector<piece> CropToPieces::crop2pieces2(Mat& srcImage,int rows,int cols){
    /**
     * params：
     * @rows x方向条数
     * @cols y方向条数
     *
     * 1. +1 细条数目
     * 2. 两条并一条 计算边界范围
     * 3. 生成切片
     */

    vector<piece> pool;

    stringstream ouou;
    ouou<<"TPR"<<rows<<"TPC"<<cols;
    _cuttype = ouou.str();

    int piece_width = srcImage.cols/(cols+1);
    int piece_height = srcImage.rows/(rows+1);

    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){

            piece temp_piece;

            int startx = col * piece_width;
            int endx = (col+2) * piece_width;
            int starty = row * piece_height;
            int endy = (row+2) * piece_height;

            getSafeBoundery(0,srcImage.cols-1,startx);
            getSafeBoundery(0,srcImage.cols-1,endx);
            getSafeBoundery(0,srcImage.rows-1,starty);
            getSafeBoundery(0,srcImage.rows-1,endy);

            Mat temp_mat;
            srcImage(Rect(startx,starty,endx-startx,endy-starty)).copyTo(temp_mat);

//            imshow("hahsdhfa",piece_temp);
//            waitKey(0);

            temp_piece.piece_mat = temp_mat;
            temp_piece.left_top = Point(startx,starty);
            pool.push_back(temp_piece);
        }
    }

    return pool;

}

void CropToPieces::writeresults(vector<piece>& pool){

    int piececount=0;

    for(int i=0;i<pool.size();i++){
        if(!pool[i].piece_mat.empty()){
            stringstream oo;
            oo<<_savedir<<"/"<<_imgcount<<"_"<<_cuttype<<"_"<<piececount<<".jpg";
            //oo<<_savedir<<"/"<<_imagename<<"_"<<_cuttype<<"_"<<piececount<<".jpg";
            imwrite(oo.str(),pool[i].piece_mat);
            piececount++;
        }
    }
    
}

void CropToPieces::setImageName(string imagename){
    _imagename = imagename;
}

