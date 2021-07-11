#ifndef TRACKBARS_H
#define TRACKBARS_H

#endif // TRACKBARS_H
using namespace cv;

const int Max_Slider(255);

Scalar Lower_Threshhold = (0,0,0);
Scalar Upper_Threshhold = (0,0,0);

int slider_LB = 0;
int slider_LG = 0;
int slider_LR = 0;
int slider_UB = 0;
int slider_UG = 0;
int slider_UR = 0;

static void on_trackbar_Lower_Blue( int, void*) { Lower_Threshhold[0] = slider_LB;}
static void on_trackbar_Lower_Green( int, void*){ Lower_Threshhold[1] = slider_LG;}
static void on_trackbar_Lower_Red( int, void*)  { Lower_Threshhold[2] = slider_LR;}
static void on_trackbar_Upper_Blue( int, void*) { Upper_Threshhold[0] = slider_UB;}
static void on_trackbar_Upper_Green( int, void*){ Upper_Threshhold[1] = slider_UG;}
static void on_trackbar_Upper_Red( int, void*)  { Upper_Threshhold[2] = slider_UR;}




