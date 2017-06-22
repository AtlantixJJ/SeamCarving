#ifndef SCALECARVER_H_
#define SCALECARVER_H_
#include "carver/simplecarver.h"

class ScalingCarver : public SimpleCarver
{
protected:
    void Scale();


protected:
    int scale_w,scale_h; // first scale to this 
    bool use_scale;
    double lam;
public:
    /// lam : the ratio of scale
    ScalingCarver(cv::Mat im, SIGNAL vb, double lam=0.5) : 
        SimpleCarver(im,vb)
    {
        printf("Scaling Carver Initiating...\n");
        this->lam = lam;
    }

    virtual void carve(int choice){
        Scale();
        SimpleCarver::carve(choice);
    }


};

#endif