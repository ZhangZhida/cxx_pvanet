//
// Created by cuizhou on 18-3-14.
//

#ifndef CXX_PVA_INTERFACE_NMSTHRESHFILTERBOXES_H
#define CXX_PVA_INTERFACE_NMSTHRESHFILTERBOXES_H

#include <opencv2/core/core.hpp>
#include "RectangleOperation.h"
#include "pvaDetector.h"

using namespace cv;
using namespace std;
class NmsThreshFilterBoxes {
public:
    NmsThreshFilterBoxes(int);
    void filter(vector<Rect>&);
    void filter(vector<Detection>&);
};


#endif //CXX_PVA_INTERFACE_NMSTHRESHFILTERBOXES_H
