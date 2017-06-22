#ifndef SIMPLECARVER_H_
#define SIMPLECARVER_H_

#include "carver/basecarver.h"
#include <stdio.h>
using namespace std;

/**
* Implemented a naive version of SeamCarving
**/
class SimpleCarver : public BaseCarver{
protected:
	//Compute the full energy matrix by scanning the whole image
	void computeFullEnergy();
	void carveHorizontal(int times);
	void carveVertical(int times);
	//void fillHorizontal(int times);
	//void fillVertical(int times);

	//Find the optimal seams
	void findVerticalSeam(vector<uint>*);
	void findHorizontalSeam(vector<uint>*);

	void removeVerticalSeam(vector<uint> *seam);
	void removeHorizontalSeam(vector<uint> *seam);
	void fillVerticalSeam(vector<uint> *seam);
	void fillHorizontalSeam(vector<uint> *seam);

    /// DEBUG
	void showVerticalSeam(vector<uint> seam);
	void showHorizontalSeam(vector<uint> seam);

public:
	virtual void carve(int choice);

	SimpleCarver(cv::Mat im, SIGNAL vb) :
		BaseCarver(im, vb){
		computeFullEnergy();
	}

	virtual ~SimpleCarver() {
	}

private:
	void Sobel3();
	void Sobel5();
	void Laplace3();
	void Laplace5();
	void MinDist();
	void MinSquired();
	void defaultKernel();
	
	//cv::Mat image;
	//cv::Mat temp;
	//cv::Mat energy;
	//int tw,th;
};

#endif /* SEAMCARVER_H_ */
