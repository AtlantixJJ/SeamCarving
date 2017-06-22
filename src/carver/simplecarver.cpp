#include "carver/simplecarver.h"

void SimpleCarver::showVerticalSeam(vector<uint> seam) {
	cv::Mat tmp;
	image.copyTo(tmp);
	for (int i = 0; i < tmp.rows; ++i)
		tmp.at<cv::Vec3b>(i, seam[i]) = cv::Vec3b(0, 0, 255);	//Set the color of the seam to Red
	cv::imshow("Seam", tmp);
	tmp.release();
}

void SimpleCarver::showHorizontalSeam(vector<uint> seam) {
	cv::Mat tmp;
	image.copyTo(tmp);
	for (int i = 0; i < tmp.cols; ++i)
		tmp.at<cv::Vec3b>(seam[i], i) = cv::Vec3b(0, 0, 255);	//Set the color of the seam to Red
	cv::imshow("Seam", tmp);
	tmp.release();
}

void SimpleCarver::carve(int choice){
	this->choice = choice;
    int dw = image.cols - tw;
    int dh = image.rows - th;
    printf("%d %d %d,%d\n",image.cols, image.rows, dw,dh);
    carveHorizontal( dh );
	carveVertical( dw );
    // NOTICE : DEBUG
    
}

void SimpleCarver::carveVertical(int times){
    int i;
    vector<uint> seam(image.rows);
	bool flag = times>0;
	if(!flag)times = -times;
    for(i = 0; i < times ; i++){
		printf("Ver:%d\n",i);
        findVerticalSeam(&seam);
        if(flag)removeVerticalSeam(&seam);
		else fillVerticalSeam(&seam);
    }
}

void SimpleCarver::carveHorizontal(int times){
    int i;
    vector<uint> seam(image.cols);
	bool flag = times > 0;
	if(!flag)times = -times;
	cv::transpose(image, image);
	cv::transpose(energy, energy);
    for(i = 0; i < times ; i++){
		printf("Hor:%d\n",i);
        findVerticalSeam(&seam);
        if(flag)removeVerticalSeam(&seam);
		else fillVerticalSeam(&seam);
    }
	cv::transpose(image, image);
	cv::transpose(energy, energy);
}

void SimpleCarver::computeFullEnergy() {
	switch(choice){
		case 0:
			defaultKernel();
		break;
		case 1:
			Sobel3();
		break;
		case 2:
			Sobel5();
		break;
		case 3:
			Laplace3();
		case 4:
			Laplace5();
		case 5:
			MinDist();
			break;
		case 6:
			MinSquired();
			break;
		default:
		break;
	}
	//printf("%d %d\n",energy.rows, energy.cols);
}

void SimpleCarver::Laplace3()
{
	int k = 3, s = 1, d = 0;
	cv::Laplacian(image, energy, 3, k, s, d);
	int i,j;
	for(i = 0;i < energy.rows; i++){
		for(j = 0; j < energy.cols ; j++){
			energy.at<uint32_t>(i,j) = energy.at<uint32_t>(i,j) * energy.at<uint32_t>(i,j);
		}
	}
}

void SimpleCarver::Laplace5()
{
	int k = 5, s = 1, d = 0;
	cv::Laplacian(image, energy, 3, k, s, d);
	int i,j;
	for(i = 0;i < energy.rows; i++){
		for(j = 0; j < energy.cols ; j++){
			energy.at<uint32_t>(i,j) = energy.at<uint32_t>(i,j) * energy.at<uint32_t>(i,j);
		}
	}
}

void SimpleCarver::defaultKernel()
{
	energy.create(image.rows, image.cols, CV_32S);

	//Scan through the image and update the energy values. Ignore boundary pixels.
	for (int i = 1; i < image.rows-1; ++i) {
		uchar* prev = image.ptr<uchar>(i-1);	//Pointer to previous row
		uchar* curr = image.ptr<uchar>(i);		//Pointer to current row
		uchar* next = image.ptr<uchar>(i+1);	//Pointer to next row

		for (int j = 1; j < (image.cols)-1; ++j) {
			int val = 0;

			//Energy along the x-axis
			val += (prev[3*j]-next[3*j]) * (prev[3*j]-next[3*j]);
			val += (prev[3*j+1]-next[3*j+1]) * (prev[3*j+1]-next[3*j+1]);
			val += (prev[3*j+2]-next[3*j+2]) * (prev[3*j+2]-next[3*j+2]);

			//Energy along the y-axis
			val += (curr[3*j+3]-curr[3*j-3]) * (curr[3*j+3]-curr[3*j-3]);
			val += (curr[3*j+4]-curr[3*j-2]) * (curr[3*j+4]-curr[3*j-2]);
			val += (curr[3*j+5]-curr[3*j-1]) * (curr[3*j+5]-curr[3*j-1]);
			energy.at<uint32_t>(i, j) = val;
		}
	}
}

void SimpleCarver::MinSquired(){
	int i,j,dx,dy;
	double sum = 0;
	energy.create(image.rows, image.cols, CV_32S);

	for(i = 1;i < energy.rows-1; i++){
		for(j = 1; j < energy.cols-1 ; j++){
			sum = 0;
			for(dx = -1 ; dx <= 1 ;dx ++)
				for(dy = -1;dy<=1 ;dy ++)
					for(int id=0;id<3;id++)
					sum += ( (image.at<cv::Vec3b>(i+dx,j+dy))[id] - (image.at<cv::Vec3b>(i,j))[id] ) *
						( (image.at<cv::Vec3b>(i+dx,j+dy))[id] - (image.at<cv::Vec3b>(i,j))[id] ) ;
			energy.at<uint32_t>(i,j) = sum;
		}
	}
}

void SimpleCarver::MinDist(){
	int i,j,dx,dy,fx,fy,idx,x,y;
	int dirc[4][2]={{1,0},{-1,0},{0,1},{0,-1}};
	double cet = 0,sum;
	energy.create(image.rows, image.cols, CV_32S);

	for(i = 2; i < energy.rows-2; i++){
		for(j = 2; j < energy.cols-2 ; j++){
			for(idx = 0; idx < 4 ; idx++){
				x = i + dirc[idx][0];
				y = j + dirc[idx][1];
				sum = 0;
				cet = 1000;
				for(dx = -1 ; dx <= 1 ;dx ++)
					for(dy = -1;dy<=1 ;dy ++){
						for(int id=0;id<3;id++)
							if(x+dx == i && y + dy == j)
								sum += ( (image.at<cv::Vec3b>(i-dirc[idx][0],j-dirc[idx][1]))[id] - (image.at<cv::Vec3b>(i,j))[id] ) *
									( (image.at<cv::Vec3b>(i-dirc[idx][0],j-dirc[idx][1]))[id] - (image.at<cv::Vec3b>(i,j))[id] );
							else
								sum += ( (image.at<cv::Vec3b>(x+dx,y+dy))[id] - (image.at<cv::Vec3b>(x,y))[id] ) *
								( (image.at<cv::Vec3b>(x+dx,y+dy))[id] - (image.at<cv::Vec3b>(x,y))[id] ) ;
					}
				if(cet < sum)cet = sum;
			}
			energy.at<uint32_t>(i,j) = cet;
		}
	}
}

void SimpleCarver::Sobel3(){
	int ksize = 3;
	double scale = 1;

	cv::Sobel(image, energy, 3, 1, 1, ksize, scale);
	int i,j;
	for(i = 0;i < energy.rows; i++){
		for(j = 0; j < energy.cols ; j++){
			energy.at<uint32_t>(i,j) = energy.at<uint32_t>(i,j) * energy.at<uint32_t>(i,j);
		}
	}
}

void SimpleCarver::Sobel5(){
	int ksize = 5;
	double scale = 1;

	cv::Sobel(image, energy, 3, 1, 1, ksize, scale);
	int i,j;
	for(i = 0;i < energy.rows; i++){
		for(j = 0; j < energy.cols ; j++){
			energy.at<uint32_t>(i,j) = energy.at<uint32_t>(i,j) * energy.at<uint32_t>(i,j);
		}
	}
}

void SimpleCarver::findVerticalSeam(vector<uint> *seam) {
	unsigned int distTo[image.rows][image.cols];	//Save the shortest distance from any of the top pixels
	short edgeTo[image.rows][image.cols];			//Which of the the three top pixels, the shortest path came from

	//Initialize the distance and edge matrices
	for (int i = 0; i < image.rows; ++i) {
		for (int j = 0; j < image.cols; ++j) {
			if (i == 0)		distTo[i][j] = 0;
			else			distTo[i][j] = numeric_limits<unsigned int>::max();
			edgeTo[i][j] = 0;
		}
	}

	// Relax the edges in topological order
	for (int row = 0; row < image.rows-1; ++row) {
		for (int col = 0; col < image.cols; ++col) {
			//Check the pixel to the bottom-left
			if (col != 0)
				if (distTo[row+1][col-1] > distTo[row][col] + getEnergy(row+1, col-1)) {
					distTo[row+1][col-1] = distTo[row][col] + getEnergy(row+1, col-1);
					edgeTo[row+1][col-1] = 1;
				}
			//Check the pixel right below
			if (distTo[row+1][col] > distTo[row][col] + getEnergy(row+1, col)) {
				distTo[row+1][col] = distTo[row][col] + getEnergy(row+1, col);
				edgeTo[row+1][col] = 0;
			}
			//Check the pixel to the bottom-right
			if (col != image.cols-1)
				if (distTo[row+1][col+1] > distTo[row][col] + getEnergy(row+1, col+1)) {
					distTo[row+1][col+1] = distTo[row][col] + getEnergy(row+1, col+1);
					edgeTo[row+1][col+1] = -1;
				}
		}
	}

	//Find the bottom of the min-path
	unsigned int min_index = 0, min = distTo[image.rows-1][0];
	for (int i = 1; i < image.cols; ++i)
		if (distTo[image.rows-1][i] < min) {
			min_index = i;
			min = distTo[image.rows-1][i];
		}

	//Retrace the min-path and update the 'seam' vector
	(*seam)[image.rows-1] = min_index;
	for (int i = image.rows-1; i > 0; --i)
		(*seam)[i-1] = (*seam)[i] + edgeTo[i][(*seam)[i]];
}

void SimpleCarver::removeVerticalSeam(vector<uint>* seam) {
	//Move all the pixels to the right of the seam, one pixel to the left
	for (int row = 0; row < image.rows; ++row) {
		for (int col = (*seam)[row]; col < image.cols-1; ++col){
			image.at<cv::Vec3b>(row, col) = image.at<cv::Vec3b>(row, col+1);
            //energy.at<uint32_t>(row, col) = energy.at<uint32_t>(row, col+1);
        }
	}

	//Resize the image to remove the last column
	image = image(cv::Rect(0, 0, image.cols-1, image.rows));
    //energy = image(cv::Rect(0, 0, image.cols-1, image.rows));

	//Re-compute the energy of the new image
	computeFullEnergy();
//	computeEnergyAfterSeamRemoval(seam);
}

void SimpleCarver::fillVerticalSeam(vector<uint>* seam)
{
	cv::Mat newim;
	newim.create(image.rows, image.cols+1, CV_8UC3);
	for (int row = 0; row < image.rows; ++row) {
		for (int col = 0; col <= (*seam)[row]; ++col){
			newim.at<cv::Vec3b>(row, col) = image.at<cv::Vec3b>(row, col);
            //energy.at<uint32_t>(row, col) = energy.at<uint32_t>(row, col+1);
        }
	}
	for (int row = 0; row < image.rows; ++row) {
		for(int j=0;j<3;j++)
			newim.at<cv::Vec3b>(row, (*seam)[row] + 1)[j] = (image.at<cv::Vec3b>(row, (*seam)[row])[j] +
				 image.at<cv::Vec3b>(row, (*seam)[row]+1)[j] ) / 2;
	}

	for (int row = 0; row < image.rows; ++row) {
		for (int col = (*seam)[row]+1; col < image.cols; ++col){
			//printf("%d %d\n",row, col);
			newim.at<cv::Vec3b>(row, col+1) = image.at<cv::Vec3b>(row, col);
            //energy.at<uint32_t>(row, col) = energy.at<uint32_t>(row, col+1);
        }
	}

	image = newim.clone();
	computeFullEnergy();
}

void SimpleCarver::fillHorizontalSeam(vector<uint>* seam)
{
	cv::transpose(image, image);
	cv::transpose(energy, energy);
	fillVerticalSeam(seam);

	//Transpose back
	cv::transpose(image, image);
	cv::transpose(energy, energy);
}

void SimpleCarver::findHorizontalSeam(vector<uint> *seam) {
	//Transpose the matrices and find the vertical seam
	cv::transpose(image, image);
	cv::transpose(energy, energy);
	findVerticalSeam(seam);

	//Transpose back
	cv::transpose(image, image);
	cv::transpose(energy, energy);
}

void SimpleCarver::removeHorizontalSeam(vector<uint> *seam) {
	//Transpose the matrices and remove the vertical seam
	cv::transpose(image, image);
	cv::transpose(energy, energy);
	removeVerticalSeam(seam);

	//Transpose back
	cv::transpose(image, image);
	cv::transpose(energy, energy);
}
