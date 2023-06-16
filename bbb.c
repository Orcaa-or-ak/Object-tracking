#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./headers/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./headers/stb_image_write.h"

unsigned char *uc_arrayNew_1d(int _size){
    return (unsigned char *)calloc(_size, sizeof(unsigned char));
}

void crop_image(int start, unsigned char *img, int wi, int width, int height, int channel, int *end, unsigned char *crop)
{   
    int m,n,c;
    n = start; c = 0; m = n;
    
    for (int i = 0; i < width*height*channel; i++) {
        crop[i] = img[n];
        c++;
        n++;
        if (c == width*channel){
            n = m;
            n += wi;
            c = 0;
            m = n;
        }
    }
    *end = n;
}

void border(int start, unsigned char *img, int wi, int width, int height, int channel, int end)
{   
    int m,n,c, count;
    n = start; c = 0; m = n; count = 0;
    for (int i = 0; i < width*height*channel; i++){
        //top and left aspect
        if (count == 0 || count == height - 1) {
            img[n] = 255;
            img[n + 1] = 255;
            img[n + 2] = 255;
        }
        //left aspect
        if (c == 1){
            img[n - 1] = 255;
            img[n] = 255;
            img[n + 1] = 255;
        }
        //bottom aspect
        if (c == width*channel){
            img[n] = 255;
            img[n + 1] = 255;
            img[n + 2] = 255;
            n = m;
            n += wi;
            c = 0;
            m = n;
            count++;
        }
        c++;
        n++;
    }
}

float cos_sim(unsigned char A[], unsigned char B[], unsigned int Vector_Length)
{
    int dot = 0, denom_a = 0, denom_b = 0;
     for(int i = 0; i < Vector_Length; i++) {
        dot += A[i] * B[i] ;
        denom_a += A[i] * A[i] ;
        denom_b += B[i] * B[i] ;
    }
    return (float) dot / (sqrt((float)denom_a) * sqrt((float)denom_b)) ;
}

void process(unsigned char *img, unsigned char *temp, int widthimg, int heightimg, int widthttemp, int heighttemp){   
    int channel = 3;
    unsigned char *test = uc_arrayNew_1d(widthttemp * heighttemp * channel);
    unsigned char *temp2 = uc_arrayNew_1d(widthttemp * heighttemp * channel);
    int start, end;
    int m,n,c,n_end;
    n = 0; c = 0; m = n;
    float t, max = -2.0;
    int x = heightimg - heighttemp;
    int y = widthimg - widthttemp;
    int z = widthimg*(x-1)+y; //640x(480-214-1)+(640-160) = 170080 -> 170080 x 3 = 510240

    int pixel = 5;
    
    int step = pixel*channel;
    while (n <= z*channel){
        if (c == y*channel){
            n = m;
            n += widthimg*step;
            c = 0;
            m = n;
        }
        crop_image(n, img, widthimg*3, widthttemp, heighttemp, channel,&n_end, test);

        t = cos_sim(test,temp,heighttemp*widthttemp*channel);
        if (t >= max) {
            max = t;
            start = n - 3;
            end = n_end;
        }

        c++; 
        n += step;
    }    
    border(start, img, widthimg*3, widthttemp, heighttemp, channel, end);
    crop_image(start, img, widthimg*3, widthttemp, heighttemp, channel,&n_end, temp);
}

int main(){
    //declare vars
    int widthimg, heightimg, widthttemp, heighttemp, channelimg, channeltemp;

    char templ[30] = "./template/template";
    char path[30] = "./images/img";
    char jpg[5] = ".jpg";

    char png[5] = ".png";
    char save[30] = "./result/result";

    char path_img[50];
    char save_path[50];
    char t[10];
    
    int start = 0;
    int end = 0;

    //read template
    char path_temp[50] = "./template.jpg";
    unsigned char *temp = stbi_load(path_temp, &widthttemp, &heighttemp, &channeltemp, 0);

    for (int i = start; i <= end; i++){
        itoa(i,t,10);        
 
        //process img's path
        strcpy(path_img,path);
        strcat(path_img,t);
        strcat(path_img,jpg);

        //read image
        unsigned char *img = stbi_load(path_img, &widthimg, &heightimg, &channelimg, 0);

        //process image
        process(img, temp, widthimg, heightimg, widthttemp, heighttemp); 

        //process save template
        strcpy(path_temp,templ);
        strcat(path_temp,t);
        strcat(path_temp,png);

        //save template
        stbi_write_png(path_temp, widthttemp, heighttemp, channeltemp, temp, widthttemp * channeltemp);

        //process save path img
        strcpy(save_path,save);
        strcat(save_path,t);
        strcat(save_path,png);

        //save image
        stbi_write_png(save_path, widthimg, heightimg, channelimg, img, widthimg * channelimg);
        printf("\nNew image save to %s", save_path);
    }
}