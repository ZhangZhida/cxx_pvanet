//
// Created by cuizhou on 18-3-14.
//

#include "NmsThreshFilterBoxes.h"


NmsThreshFilterBoxes::NmsThreshFilterBoxes(int i){

}

void NmsThreshFilterBoxes::filter(vector<Rect>& candidates){
    RectangleOperation  rectangle_operation(0);
    for(int i=0;i<candidates.size();i++){
        for(int j=i+1;j<candidates.size();j++){
            Rect rect1=candidates[i];
            Rect rect2=candidates[j];
            double iou=rectangle_operation.calcOverlapRatio(rect1,rect2);
            //cout<<"candidates.size() = "<<candidates.size()<<endl;
            //cout<<"iou = "<<iou<<endl;
            if(iou>0.8){
                //cout<<">0.8"<<endl;
                double i_area=rect1.width*rect1.height;
                double j_area=rect2.width*rect2.height;
                if(i_area>j_area){
                    //删除j,j-1
                    //cout<<"//删除j,j-1"<<endl;
                    candidates.erase(candidates.begin()+j);
                    j-=1;
                }else{
                    //删除i,i-1,break
                    //cout<<"//删除i,i-1,break"<<endl;
                    candidates.erase(candidates.begin()+i);
                    i-=1;
                    break;
                }
            }
        }
    }
}

void NmsThreshFilterBoxes::filter(vector<Detection>& candidates){
    RectangleOperation  rectangle_operation(0);
    for(int i=0;i<candidates.size();i++){
        for(int j=i+1;j<candidates.size();j++){
            Rect rect1=candidates[i].getRect();
            Rect rect2=candidates[j].getRect();
            double iou=rectangle_operation.calcOverlapRatio(rect1,rect2);
            //cout<<"candidates.size() = "<<candidates.size()<<endl;
            //cout<<"iou = "<<iou<<endl;
            if(iou>0.8){
                //cout<<">0.8"<<endl;
                double i_area=rect1.width*rect1.height;
                double j_area=rect2.width*rect2.height;
                if(i_area>j_area){
                    //删除j,j-1
                    //cout<<"//删除j,j-1"<<endl;
                    candidates.erase(candidates.begin()+j);
                    j-=1;
                }else{
                    //删除i,i-1,break
                    //cout<<"//删除i,i-1,break"<<endl;
                    candidates.erase(candidates.begin()+i);
                    i-=1;
                    break;
                }
            }
        }
    }
}