#include <stdio.h>
#include "carver/simplecarver.h"
#include "carver/scalingcarver.h"
#include <cstring>  

int main(int argc, char** argv )
{
    printf("Simple Seam Carver Initiating...\n");
    if ( argc != 6 )
    {
        printf("usage: SeamCarver <Image_Path> <kernel_choice> <new_h> <new_w> <output_path>\n");
        return -1;
    }

    cv::Mat image;
    image = cv::imread( argv[1], 1 );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    printf("Image loaded (%d, %d)\n", image.rows, image.cols);
    int h = atoi(argv[3]), w = atoi(argv[4]);
    printf("Carve to (%d, %d)\n", h, w);
    printf("Using kernel %d\n",atoi(argv[2]));
    //BaseCarver *engine = new SimpleCarver(image, 1);
    BaseCarver *engine = new ScalingCarver(image, 1, 0.5);
    //engine->showImage();
    engine->setNewSize(w,h);
    engine->carve(atoi(argv[2]));
    engine->imsave(argv[5]);
    return 0;
}