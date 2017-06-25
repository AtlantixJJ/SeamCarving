#include "carver/simplecarver.h"
#include <stdlib.h>
#include <cstring>


int abs(int x){return x < 0 ? -x : x;}

void SimpleCarver::carve(int choice){
	printf("Carving Begin...\n");
	int i,j;

	for(i = 0; i< image.rows; i++){
		for( j = 0 ; j < image.cols; j++){
			carvedx[i][j] = i; carvedy[i][j] = j;
		}
	}

	this->choice = choice;
    int dw = image.cols - tw;bool wfill = dw < 0;
    int dh = image.rows - th;bool hfill = dh < 0;
	dw = abs(dw); dh = abs(dh);
	int com = dw < dh ? dw : dh;
	int last = (dw > dh ? dw : dh ) - com;
	bool flag = dw > dh;
    printf("%d %d %d,%d\n",image.cols, image.rows, dw,dh);

	for(int i = 0; i < com ; i ++){
		printf("Comm:%d\n",i);
		carveVertical(wfill?-1:1);
		carveHorizontal(hfill?-1:1);
		
	}
	for(int i = 0; i < last ; i++){
		if(flag) carveVertical(wfill?-1:1);
		else carveHorizontal(hfill?-1:1);
	}    
}

void SimpleCarver::carveVertical(int times){
    int i;
    vector<uint> seam(image.rows);
	bool flag = times>0;
	if(!flag)times = -times;
    for(i = 0; i < times ; i++){
		//printf("Ver:%d\n",i);
		dpVertical(&seam);
        //findVerticalSeam(&seam);
        if(flag)removeVerticalSeam(&seam);
		else fillVerticalSeam(&seam);
		//seam.clear();
    }
	//BaseCarver::showSeams(false);
}

void SimpleCarver::carveHorizontal(int times){
    int i;
    vector<uint> seam(image.cols);
	bool flag = times > 0;
	if(!flag)times = -times;
	cv::transpose(image, image);
	cv::transpose(grayimage, grayimage);
	cv::transpose(energy, energy);
    for(i = 0; i < times ; i++){
		//printf("Hor:%d",i);
        //findVerticalSeam(&seam);
        dpVertical(&seam);
		if(flag)removeVerticalSeam(&seam,false);
		else fillVerticalSeam(&seam);
		//seam.clear();
		//printf(" |\n");
    }
	cv::transpose(grayimage, grayimage);
	cv::transpose(image, image);
	cv::transpose(energy, energy);
}

void SimpleCarver::computeFullEnergy() {
	switch(choice){
		case 1:
			SobelFilter(3);
		break;
		case 2:
			SobelFilter(5);
		break;
		case 3:
			LaplaceFilter(3);
			break;
		case 4:
			LaplaceFilter(5);
			break;
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

string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

void SimpleCarver::SobelFilter(int k){
	int delta=0;
	int ddepth = CV_16S;
	double scale = 1;
	cv::Mat grad_x, grad_y, abs_grad_x, abs_grad_y;

	Sobel( grayimage, grad_x, ddepth, 1, 0, k, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );
	Sobel( grayimage, grad_y, ddepth, 0, 1, k, scale, delta, cv::BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, energy );
}

void SimpleCarver::LaplaceFilter(int k)
{
	int s = 1, d = 0;
	int ddepth = CV_16S;
	cv::Mat temp;
	//cv::imshow("e",grayimage);
	//cv::waitKey(0);
	Laplacian(grayimage, temp, ddepth, k, s, d, cv::BORDER_DEFAULT );
	convertScaleAbs( temp, energy );
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
			energy.at<uint8_t>(i,j) = sum;
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
							if(x+dx == i && y + dy == j)
								sum += ( (grayimage.at<uint8_t>(i-dirc[idx][0],j-dirc[idx][1])) - (grayimage.at<uint8_t>(i,j)) ) *
									( (grayimage.at<uint8_t>(i-dirc[idx][0],j-dirc[idx][1])) - (grayimage.at<uint8_t>(i,j)) );
							else
								sum += ( (grayimage.at<uint8_t>(x+dx,y+dy)) - (grayimage.at<uint8_t>(x,y)) ) *
								( (grayimage.at<uint8_t>(x+dx,y+dy)) - (grayimage.at<uint8_t>(x,y)) ) ;
					}
				if(cet < sum)cet = sum;
			}
			energy.at<uint8_t>(i,j) = cet;
		}
	}
}

void SimpleCarver::dpVertical(vector<uint> *seam)
{
	//printf("E:%d %d I %d %d\n",energy.rows,energy.cols,image.rows,image.cols);
	int i,j;
	//for(i = 0; i < image.cols ;i++){
		//for(j = 0;j < image.cols ;j++){
	//		f[0][i] = this->getEnergy(0,i);
		//}
	//}

	for(i = 0; i < image.rows ;i++){
		for(j = 0;j < image.cols ;j++){
			f[i][j] = this->getEnergy(i,j);
			//printf("%d ",f[i][j]);
		}
		//printf("\n");
	}

	int mini=10000, ind;
	for(i = 1 ; i < image.rows; i++){
		//printf("D(%d)\n",i);
		for(j = 0; j < image.cols; j++){
			if ( j >= 1){//initial
				mini = f[i-1][j-1];
				ind = j-1;
			}else{
				ind = j;
				mini = 1000000;
			}

			if(f[i-1][j] <= mini){//update
				mini = f[i-1][j];
				ind = j;
			}

			if( j < image.cols - 1 && f[i-1][j+1] <= mini){
				mini = f[i-1][j+1];
				ind = j+1;
			}

			bt[i][j] = ind;
			f[i][j] += mini;
		}
	}

	mini = 1e9;
	for(i = 0; i < image.cols; i++){
		//printf("%d ",f[image.rows-1][i]);
		if(f[image.rows-1][i] < mini){
			mini = f[image.rows-1][i];
			ind = i;
		}
	}
	//printf("\n");

	/// backtrace
	int cur = ind;
	for(i = image.rows-1 ; i >= 0 ;i--){
		//printf("Cur : %d\tI : %d\n",cur, i );
		(*seam)[i] = cur;
		cur = bt[i][cur];
	}
	
	//printf("DPDone.\n");
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

void SimpleCarver::removeVerticalSeam(vector<uint>* seam,bool flag) {
	//Move all the pixels to the right of the seam, one pixel to the left
	for (int row = 0; row < image.rows; ++row) {
		for (int col = (*seam)[row]; col < image.cols-1; ++col){
			image.at<cv::Vec3b>(row, col) = image.at<cv::Vec3b>(row, col+1);
			if(flag){
				carvedx[row][col] = carvedx[row][col+1];
				carvedy[row][col] = carvedy[row][col+1];
			}else{
				//carvedx[row][col] = carvedx[row][col+1];
				//carvedy[row][col] = carvedy[row][col+1];
				carvedx[col][row] = carvedx[col+1][row];
				carvedy[col][row] = carvedy[col+1][row];
			}
			//grayimage.at<uint8_t>(row, col) = grayimage.at<uint8_t>(row, col+1);
            //energy.at<uint32_t>(row, col) = energy.at<uint32_t>(row, col+1);
        }
	}

	//Resize the image to remove the last column
	image = image(cv::Rect(0, 0, image.cols-1, image.rows));
	cv::cvtColor(image,grayimage,CV_RGB2GRAY);
    //energy = image(cv::Rect(0, 0, image.cols-1, image.rows));

	//Re-compute the energy of the new image
	computeFullEnergy();
//	computeEnergyAfterSeamRemoval(seam);
}

void SimpleCarver::fillVerticalSeam(vector<uint>* seam)
{
	//printf("Fill V being...\n");
	cv::Mat newim;
	newim.create(image.rows, image.cols+1, CV_8UC3);
	for (int row = 0; row < image.rows; ++row) {
		if((*seam)[row] >= image.cols){
			printf("Out of bound. %d %d\n",(*seam)[row],image.cols);
		}
		for (int col = 0; col <= (*seam)[row]; ++col){
			newim.at<cv::Vec3b>(row, col) = image.at<cv::Vec3b>(row, col);
            //energy.at<uint32_t>(row, col) = energy.at<uint32_t>(row, col+1);
        }
	}
	for (int row = 0; row < image.rows; ++row) {
		for(int j=0;j<3;j++)
			if((*seam)[row] + 1 < image.cols)
				newim.at<cv::Vec3b>(row, (*seam)[row] + 1)[j] = (image.at<cv::Vec3b>(row, (*seam)[row])[j] +
				 image.at<cv::Vec3b>(row, (*seam)[row]+1)[j] ) / 2;
			else
				newim.at<cv::Vec3b>(row, (*seam)[row] + 1)[j] = (image.at<cv::Vec3b>(row, (*seam)[row]-1)[j] +
				 image.at<cv::Vec3b>(row, (*seam)[row])[j] ) / 2;
	}

	for (int row = 0; row < image.rows; ++row) {
		for (int col = (*seam)[row]+1; col < image.cols; ++col){
			//printf("%d %d\n",row, col);
			newim.at<cv::Vec3b>(row, col+1) = image.at<cv::Vec3b>(row, col);
            //energy.at<uint32_t>(row, col) = energy.at<uint32_t>(row, col+1);
        }
	}

	image = newim.clone();
	cv::cvtColor(newim,grayimage,CV_RGB2GRAY);

	//printf("Fill V end.\n");
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
