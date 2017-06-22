#include "carver/scalingcarver.h"

void ScalingCarver::Scale() {
    double r1 = tw / double(image.cols);
    double r2 = th / double(image.rows);
    double r=0;
    // if cols > tw && rows > th : take max
    // if cols < tw && rows < th : take min
    int dw = image.cols - tw; // tw is pre-setted
    int dh = image.rows - th;
    if(dw * dh < 0){
        use_scale = false;
        return;
    }

    if(dw > 0 && dh > 0){ // full carving case
        r = r1 > r2 ? r1 : r2;
        r += (1-r)*lam; // lam == 1 : full scale
    }
    

    if(dw < 0 && dh < 0){ // full filling case
        r = r1 > r2 ? r2 : r1;
        r -= (r-1)*lam;
    }

    printf("Original Size : %d %d -> %d %d\n",image.cols,image.rows,tw,th);
    printf("Ratio %.3lf %.3lf %.3lf\n",r1,r2,r);
    cv::resize(image, image, cv::Size(0,0), r, r);
    printf("Scaled to %d %d\n",image.cols,image.rows);
}