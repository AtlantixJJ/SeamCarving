#ifndef BASECARVER_H_
#define BASECARVER_H_

#include <opencv2/opencv.hpp>
#include "common.h"
using namespace std;

/**
*   BaseCarver do some preprocessings and a virtual function design.
**/
class BaseCarver {
public:
    virtual void carve(int choice) = 0;

	BaseCarver(cv::Mat im, SIGNAL verbose) {
		image = im;
		temp = image;
        this->verbose = verbose;
        tw = th = -1;
		energy = cv::Mat(image.rows, image.cols, CV_32S, cv::Scalar(195075));
	}

    /// Carving is used to 
	void setNewSize(int w, int h){
        tw = w; th = h;
    }

	virtual ~BaseCarver() {
		temp.release();
		energy.release();
	}

	//Show the image
	void showImage() {
		cv::imshow("Image", image);
		cv::waitKey(0);
	}

	//Show the energy matrix as an image
	void showEnergy() {
		cv::imshow("Energy", energy);
		cv::waitKey(0);
	}

	//Return the image
	cv::Mat getImage() {
		return image;
	}

	bool imsave(char *p){
		return cv::imwrite(cv::String(p), image);
	}

	//Return the value of the energy at a particular pixel
	unsigned int getEnergy(unsigned int row, unsigned int col) {
		return energy.at<uint32_t>(row, col);
	}

protected:
	cv::Mat image;
	cv::Mat temp;
	cv::Mat energy;
	int choice;
    SIGNAL verbose; /// control the debug information
	int th,tw;
};

#endif /* SEAMCARVER_H_ */
