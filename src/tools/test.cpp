#include <iostream>
#include <stdio.h>  
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    int n = 5,m =4;
	int **f = new int*[n];
	int i,j;
	for (i = 0 ; i < n ;i++){
		f[i] = new int[m];
	}
    for( i = 0; i< n; i++){
        for(j=0;j<m;j++)
            f[i][j] = j;
    }
    printf("%d\n",sizeof(f));
	//memset(f, 0, sizeof(f));

    for( i = 0; i< n; i++){
        for(j=0;j<m;j++)
            printf("%d ",f[i][j]);// = j;
        printf("\n");
    }

    return 0;
}