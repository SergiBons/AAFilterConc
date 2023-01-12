/*
Copyright (c) 2004, Lode Vandevenne
All rights reserved.
*/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define filterWidth 5
#define filterHeight 5
#define w 25004
#define h 25004
#define seed 101010
#define CheckSize 15

int main(int argc, char* argv[])
{
    int filter[2][filterHeight][filterWidth] =
    {
      1, 1, 1, 1, 1,
      1, 1, 1, 1, 1,
      1, 1, 1, 1, 1,
      1, 1, 1, 1, 1,
      1, 1, 1, 1, 1,

     -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1,
     -1, -1, 24, -1, -1,
     -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1,
    };


    double factor[2] = {(1.0 / 25.0), 1.0};
    double bias = 0.0;
    //load the image into the buffer
    int *image = (int*)malloc(sizeof(int) * 3 * w * h);
    int *result = (int*)malloc(sizeof(int) * 3 * w * h);
    int *temp = NULL;
    //loadImage(image, w, h, "pics/photo3.png");

    srand(seed);
    for (int k = 0; k < 3; k++)
        for (int i = 2; i < h-2; i++)
            for (int j = 2; j < w-2; j++)
                image[(k * h * w) + i * w + j] = rand() % 255;
	/*
   for (int k = 0; k < 3; k++)
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                image[(k * h * w) + i * w + j] =0;




        image[w*6+4] = 200;
        image[w*6+5] = 200;
        image[w*6+6] = 200;
        image[w*6+7] = 200;
        image[w*6+8] = 200;
        image[w*6+9] = 200;
        image[w*6+10] = 200;

        image[w*7+4] = 200;
        image[w*7+10] = 200;

        image[w*8+4] = 200;
        image[w*8+10] = 200;

        image[w*9+4] = 200;
        image[w*9+5] = 200;
        image[w*9+6] = 200;
        image[w*9+7] = 200;
        image[w*9+8] = 200;
        image[w*9+9] = 200;
        image[w*9+10] = 200;
	*/














    int weight = w-4;
    int height = h-4;
    printf("w = %d + 4,  h = %d + 4   \n", weight ,height);
    
    #pragma omp parallel
{
    #pragma omp single
{
    #pragma omp taskloop
    for (int k = 0; k < 3; k++)
        for (int i = 0; i < h; i++)
            for (int j = 0; j < 2; j++)
                image[(k * h * w) + i * w + j]= image[(k * h * w) + i * w + w-4+j];
    #pragma omp taskloop
    for (int k = 0; k < 3; k++)
        for (int i = 0; i < h; i++)
            for (int j = 0; j < 2; j++)
                image[(k * h * w) + i * w + (w+j-2)]=  image[(k * h * w) + i * w + j+2] ;
    #pragma omp taskloop
    for (int k = 0; k < 3; k++)
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < w; j++)
                image[(k*h*w) +i*w + j]= image[(k * h * w) + (h+i-4) * w + j];
    #pragma omp taskloop
    for (int k = 0; k < 3; k++)
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < w; j++)
                image[(k * h * w) + (h+i-2) * w + j]= image[(k * h * w) + (i+2) * w + j] ;

   #pragma omp taskwait
    for (int nFilters = 0; nFilters <= 1; nFilters++) {
	#pragma omp taskloop
        //apply the filter
        for (int y = 2; y < h-2; y++)
            for (int x = 2; x < w-2; x++)
            {
                int red = 0, green = 0, blue = 0;

                //multiply every value of the filter with corresponding image pixel
                for (int filterY = 0; filterY < filterHeight; filterY++)
                    for (int filterX = 0; filterX < filterWidth; filterX++)
                    {
                        int imageX = (x - filterWidth / 2 + filterX);
                        int imageY = (y - filterHeight / 2 + filterY);
                        red += image[imageY * w + imageX] * filter[nFilters][filterY][filterX];
                        green += image[w*h+imageY * w + imageX] * filter[nFilters][filterY][filterX];
                        blue += image[w*h*2+imageY * w + imageX] * filter[nFilters][filterY][filterX];
                    }

                //truncate values smaller than zero and larger than 255
                result[y * w + x] = MIN(MAX(factor[nFilters] * red + bias, 0), 255);
                result[w * h + y * w + x] = MIN(MAX(factor[nFilters] * green + bias, 0), 255);
                result[w * h * 2 + y * w + x]= MIN(MAX(factor[nFilters] * blue + bias, 0), 255);
            }
        //draw the specified points
        printf("Iteracio filtre: %d\n \n",nFilters+1);
	printf("Valor en checkpoint:\n");
	for (int checkY = 0; checkY < CheckSize; checkY++)
	{
	    for (int checkX = 0; checkX < CheckSize; checkX++)
	    	printf("%d	",result[w*checkY+checkX]);
	    printf("\n");
	}

        temp = result;
        result = image;
        image = temp;
    }
}
}
}
