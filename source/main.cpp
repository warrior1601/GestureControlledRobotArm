#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/gapi/gscalar.hpp>
#include <opencv2/gapi/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/gapi/render/render_types.hpp>

#include <fstream>
#include <iterator>

#include "handformat.h"
#include "socketsetup.h"
#include "trackBars.h"
#include "drawings.h"

using namespace cv;
using namespace std;

void Save();

vector<Point2f> True;
vector<Point2f> Circle;
vector<Point2f> rect_tan_gle;

int main(int, char**)
{
    /*
     * Set up communication with the Raspberry Pi
     */
    socketSetup();

    /* Camera_Number will be what camera is being streamed
     * "0" for Laptop built in WebCam
     * "1" for Desktop Visualizer
     */
    int Camera_Number = 0;
    int key = 0;
    bool Calibrated = false;

    cout<< "Input Camera number: " <<endl;
    cin >> Camera_Number;

    //Check to see if there is valid connection
    VideoCapture Camera_Stream(Camera_Number);
    if (!Camera_Stream.isOpened())
    {
        cout << "Can't find camera!" << endl;
        return -1;
    }
    bool saveData = false; // Used for recording Center of Plam Data
    bool record = false;   // Used for recording a video
    bool calibrationHappy = false;
    //  Create all the Matrixes that hold Picture Data
    Mat Live_View, Threshold_Applied, HSV, gaussianBlur;

    // Used for creating video writer
    int frame_width = (int) Camera_Stream.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = (int) Camera_Stream.get(cv::CAP_PROP_FRAME_HEIGHT);

    // Create a video writer
    VideoWriter video("outcpp.avi", cv::VideoWriter::fourcc('M','J','P','G'), 10, Size(frame_width,frame_height));

    while (true)
    {
        if(saveData == true)
        {
            /* On each iteration of the while loop data will be loaded into an array
             * True.push_back(True_Center);
             * Circle.push_back(center);
             * rect_tan_gle.push_back(Rect_Center);
             */
        }

        if(record == true)
        {
            // Creates the video recording
            video.write(Live_View);

        }
        // Move the camera feed into the matrix
        Camera_Stream >> Live_View;
        // Blur the image with a 7x7 matrix
        GaussianBlur(Live_View, gaussianBlur, Size(7,7), 7,7, BORDER_CONSTANT );
        cvtColor(gaussianBlur, HSV, COLOR_BGR2HSV);

        //Set up an area the user can calibrate glove color from
        Draw_Glove_Colour_Calibration_Squares(Live_View);

        //Set up commands ao the user can calibrate
        //Begin and escape the program
        key = waitKey(1);

        if (key == 27) // "esc" key pressed exits the application
        {break;}
        else if (key == 'b')
        {
            // create Background Subtractor objects
            // This function was never implemented
        }
        else if (key == 'c') // Calibrate glove or hand colour and apply threshold
        {
            Calibrate_Glove_Colour(HSV);
            Calibrated = true;
        }
        else if(key == 's') // Start saving and center data
        {
            saveData = true;
        }
        else if(key == 't') // Stop saving hand center data
        {
            saveData = false;
        }
        else if(key == 'q') // This writes the hand data to a text file
        {
            Save();
            True.clear();
            Circle.clear();
            rect_tan_gle.clear();
        }
        else if(key == 'v') // Start and stop recording
        {
            if(record == false)
            {
                record = true;
            }
            else
             {
                record = false;
             }
        }
        else if(key == 'h') // Start and stop recording
        {
            calibrationHappy = true;
        }

        /*
         * Using the Track bars or claibraion function hands can be found
         */
        inRange(HSV, Lower_Threshhold, Upper_Threshhold, Threshold_Applied);

        if(!Threshold_Applied.empty() )
        {
            on_trackbar_Lower_Blue(slider_LB,0);
            on_trackbar_Lower_Green(slider_LG,0);
            on_trackbar_Lower_Red(slider_LR,0);
            on_trackbar_Upper_Blue(slider_UB,0);
            on_trackbar_Upper_Green(slider_UG,0);
            on_trackbar_Upper_Red(slider_UR,0);

            if(Calibrated == true)
            {
                Draw_Contours(Threshold_Applied, Live_View, calibrationHappy);
            }

        }
        /*
         * This function draws quadrents on the screen
         */
        //drawAxisAndQuadents(Live_View);

        /*
         * Shows the user what the camera sees
         */
        imshow("Live View", Live_View);

        /*
         * These are used for picture collection only
         * imshow("Gaussian Blur", gaussianBlur);
         * imshow("HSV", HSV);
         * imshow("Threshold_Applied", Threshold_Applied);
         */

        createTrackbar( "LH", "Live View", &slider_LB, Max_Slider, on_trackbar_Lower_Blue );
        createTrackbar( "LS", "Live View", &slider_LG, Max_Slider, on_trackbar_Lower_Green );
        createTrackbar( "LV", "Live View", &slider_LR, Max_Slider, on_trackbar_Lower_Red );
        createTrackbar( "UH", "Live View", &slider_UB, Max_Slider, on_trackbar_Upper_Blue );
        createTrackbar( "US", "Live View", &slider_UG, Max_Slider, on_trackbar_Upper_Green );
        createTrackbar( "UV", "Live View", &slider_UR, Max_Slider, on_trackbar_Upper_Red );
    }

    //To save CPU space destroy all windows and release the memory back
    Camera_Stream.release();
    Live_View.release();
    gaussianBlur.release();
    Threshold_Applied.release();
    HSV.release();
    destroyAllWindows();

    return 0;
}

// Function Saves the center of hand data
void Save()
{
        std::ofstream output_file("./True Center.txt");
        std::ostream_iterator<Point2f> output_iterator(output_file, "\n");
        std::copy(True.begin(), True.end(), output_iterator);

        std::ofstream output_fil("./Enclosing Center.txt");
        std::ostream_iterator<Point2f> output_iterato(output_fil, "\n");
        std::copy(Circle.begin(), Circle.end(), output_iterato);

        std::ofstream output_fi("./Rectangle Center.txt");
        std::ostream_iterator<Point2f> output_iterat(output_fi, "\n");
        std::copy(rect_tan_gle.begin(), rect_tan_gle.end(), output_iterat);
}
