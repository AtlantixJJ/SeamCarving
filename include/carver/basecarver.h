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
		temp = image.clone();
		f = bt = nullptr;
		cv::GaussianBlur( image, temp, cv::Size(3,3), 0, 0, cv::BORDER_DEFAULT );
		cv::cvtColor(temp,grayimage,CV_BGR2GRAY);

        this->verbose = verbose;
        tw = th = -1;
		energy = cv::Mat(image.rows, image.cols, CV_32S, cv::Scalar(195075));
	}

	void setNewSize(int w, int h){
		printf("Setting New size(%d,%d)...\n",w,h);
		/// Take the largest to make the computation table
        tw = w; th = h;
		int sw = tw > image.cols ? tw : image.cols;
		int sh = th > image.rows ? th : image.rows;
		int d = sh > sw ? sh : sw;
		lld = d;

		f = new int*[d];
		bt = new int*[d];
		carvedx = new int*[d];
		carvedy = new int*[d];
		int i,j;
		for (i = 0 ; i < d ;i++){
			//printf("%d\n",i);
			f[i] = new int[d];
			bt[i] = new int[d];
			carvedx[i] = new int[d];
			carvedy[i] = new int[d];
		}
		for(i = 0; i < d; i++)
			for(j = 0; j < d ;j++)
				carvedx[i][j] = 0;
		printf("New size allocation done.\n");
    }
	
	/// If save=false, then imshwo the seams
	void showSeams(char *p,bool flag=false){
		int i,j;
		for(i = 0 ;i < temp.rows;i++){
			for(j = 0; j < temp.cols; j++){
				temp.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,255);
			}
		}

		for(i=0;i<image.rows;i++){
			for(j=0;j<image.cols;j++){
				temp.at<cv::Vec3b>(carvedx[i][j], carvedy[i][j]) = image.at<cv::Vec3b>(i,j);
			}
		}
		if(!flag){
			cv::imshow("Seam",temp);
			cv::waitKey(0);
		}else{
			cv::imwrite(p,temp);
		}
	}

	virtual ~BaseCarver() {
		
		for(int i = 0; i < lld ;i++){
			delete [] f[i];
			delete [] bt[i];
		}
		delete [] bt;
		delete [] f;
		
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
	 int getEnergy(unsigned int row, unsigned int col) {
		return energy.at<uint8_t>(row, col);
	}

protected:
	int lld;
	int **f,**bt,**carvedx,**carvedy;
	cv::Mat image;
	cv::Mat grayimage;
	cv::Mat temp;
	cv::Mat energy;
	int choice;
    SIGNAL verbose; /// control the debug information
	int th,tw;
};

#endif /* SEAMCARVER_H_ */
