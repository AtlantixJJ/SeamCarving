#include <iostream>
#include <stdio.h>  
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    Mat m = Mat::eye(5,5,CV_8UC1);
    cout << m << endl;
    int i,j;
    for(i = 0 ; i< m.rows; i++){
        uchar *ptr = m.ptr<uchar>(i);
        for(j = 0;j < m.cols; j++){
            printf("%d ",ptr[3*j]);
        }
        printf("\n");
    }
    return 0;
}