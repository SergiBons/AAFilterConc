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
#define w 10000
#define h 10000
#define seed 101010
#define CheckSize 10



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

double factor[2] = {(1.0 / 9.0), 1.0};
double bias = 0.0;
int RGB;
void apply_filter(int* image,int x, int y,int nFilters,int color)
{
	RGB = 0;
                for (int filterY = 0; filterY < filterHeight; filterY++)
                    for (int filterX = 0; filterX < filterWidth; filterX++)
                    {
                        int imageX = (x - filterWidth / 2 + filterX + w) % w;
                        int imageY = (y - filterHeight / 2 + filterY + h) % h;
                        RGB += image[w*h*color+imageY * w + imageX] * filter[nFilters][filterY][filterX];
                    }
}

void truncate(int * result, int x, int y, int nFilters, int color)
{
                result[w*h*color + y * w + x] = MIN(MAX(factor[nFilters] * RGB + bias, 0), 255);
}

void printCheckpoint(int * result, int nFilters)
{
        printf("Iteracio filtre: %d\n \n",nFilters+1);
	printf("Valor en checkpoint:\n");
	for (int checkY = 0; checkY < CheckSize; checkY++)
	{
	    for (int checkX = 0; checkX < CheckSize; checkX++)
	    	printf("%d	",result[w*checkY+checkX]);
	    printf("\n");	
	}

}


int main(int argc, char* argv[])
{
    //load the image into the buffer
    int *image = (int*)malloc(sizeof(int) * 3 * w * h);
    int *result = (int*)malloc(sizeof(int) * 3 * w * h);
    int *temp = NULL;

    //loadImage;
    srand(seed);
    for (int k = 0; k < 3; k++)
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                image[(k * h * w) + i * w + j] = rand() % 255;


    for (int nFilters = 0; nFilters <= 1; nFilters++) {

        //apply the filter
	for(int color = 0; color < 3; color++) 
		for (int y = 0; y < h; y++)
            		for (int x = 0; x < w; x++)
            		{
				apply_filter(image,x,y,nFilters,color);
                		//truncate values smaller than zero and larger than 255
                		truncate(result,x, y, nFilters,color);
            		}
        //draw the specified points
	printCheckpoint(result,nFilters);
        
        temp = result;
        result = image;
        image = temp;
    }

}



