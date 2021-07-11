#ifndef DRAWINGS_H
#define DRAWINGS_H

#endif // DRAWINGS_H
#include <cmath>

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
using namespace cv;

//----------------------------------------Global Variable----------------------------------------//
/*
 * Global variable are not always a good idea
 * futher version of this code will see many
 * of them as local variables. Often variables
 * are passed up a sequence of functions to either
 * demonstate a purpose or as a safety check. This
 * was avoided by making the variables global to simplify
 * the function calls.
 */
bool transmitGo = false;
// These 4 points are used to find the angle
// rotation of the hand
Point2f A;
Point2f B;
Point2f C;
Point2f RunningAverageWristAngle; // Average value of last 10 fingertip values

// These t3 points are use to detemine best
// center hand finding location 2 are commented out
// because they are no longer needed. Left in for demonstrations

// Point2f center;
// Point2f True_Center;
Point2f Rect_Center;

// For Demonstraion only
// What hand pose was detected
// string HandPose = "Not Detected";

// These variablesset safties enabling or disabling
// Movements of the arm as the hand moves across the
// viewing screen
int EnabledMovements; // Uses to control a switch statements: int comes from the "EnableMovements" Array
int EnableMovements[16] = {3,2,1,4,2,3,4,1,1,4,3,2,4,1,2,3};
/*
 * D ~ Down; U ~ Up; R ~ Right; L ~ Left
 *
 * H = 0 // Never used
 * D = 1
 * U = 2
 * L = 3
 * R = 4
 * DL =5
 * DR =6
 * UL =7
 * UR =8
 */
int DirectionalMovements[16] = {5,1,3,0,1,6,0,4,3,0,7,2,0,4,2,8}; // Translates Safety Enables to Directional Movements
// These vectors stores FingerTips and hull defects which are the inter-digital folds.
vector<Point> amend_start_pt;
vector<Point> amend_farthest_points;

//----------------------------------------Functions----------------------------------------//
// This function Determines whether the compare point is across a line formed from startPoint to EndPoint
int crossProduct(Point2f startPoint, Point2f EndPoint, Point2f Compare);

// This function assembles the command word for transmittion
void assembleCommandWord(int safetyByte);

// This function is call from inside "assembleCommandWord" function to concentrrate all char strings into one string
void ConcentrateCharArrays(char* CommandWord, char* Handpose, char* Bicep, char* Wrist, char* Shoulder);

// These functions stop adjust the command word to stop their respected movements
void StopBicep(char* Bicep);
void StopWrist(char* Wrist);
void StopShoulder(char* Shoulder);

// These functions get set the sommand word based upon the "EnabledMovements" Array value
void getBicep(char* Bicep);
void getWrist(char* Wrist);
void getShoulder(char* Shoulder);

// This function creates the runnin average of the fingertips
void AverageFingerTips();

// This function calculates the distance between two points
double Magnitude(Point2f aPoint, Point2f bPoint);

// These functions reduces the number of points dectects along the hand contour
vector<Point> Amden_Points_Start(vector<Point> &points, Point2f center);
vector<Point> Amden_Points(vector<Point> &points, Point2f center);

// Draws Squares on the screen to indicate calibration region
void Draw_Glove_Colour_Calibration_Squares(Mat &aLive_View);

// ~~~~~~This function needs work~~~~~~~~~~~~  calibrates for the average colour detected in the two squares
void Calibrate_Glove_Colour(Mat &aLive_View);

// Returns the point for the center of the hand
void Draw_Contours(Mat &aThreshold_Applied, Mat &aLiveView, bool calibrationHappy);

// Draws the Axis and Quadernts for visulaizing hand placement versus expected movement
void drawAxisAndQuadents(Mat &aLive_View);

// Will deect the hand to opening and closing
void detectHandPose(char* HandPose);

// Locates the center of the hand to detemine eligible movemnets
int setSafeties(Mat &aThreshold_Applied, Point2f center);

//----------------------------------------Function Definitions----------------------------------------//
void Draw_Glove_Colour_Calibration_Squares(Mat &aLive_View)
{
    rectangle(  aLive_View,
                Rect( int ((aLive_View.size().width) * 0.5), ((aLive_View.size().height) / 2) + 40, 20, 20),
                Scalar(0, 0, 255),
                3
                );

    rectangle(  aLive_View,
                Rect( int((aLive_View.size().width) * 0.5), ((aLive_View.size().height) / 3) + 40, 20, 20),
                Scalar(0, 0, 255),
                3
                );
}

void Calibrate_Glove_Colour(Mat &aHSV)
{
    // Creates the Mats for colour abstraction
    Mat Square_1 = Mat(aHSV, (Rect( int((aHSV.size().width) *0.5), ((aHSV.size().height) / 2) + 40, 20, 20)));
    Mat Square_2 = Mat(aHSV, (Rect( int((aHSV.size().width) *0.5), ((aHSV.size().height) / 3) + 40, 20, 20)));

    // Takes the mean HSV of those squares
    Scalar Square_1_mean = mean(Square_1);
    Scalar Square_2_mean = mean(Square_2);

    // Averge the means
    Square_1_mean = (Square_1_mean + Square_2_mean) * 0.5;
    // cout << "Mean Value of Squares: " <<Square_1_mean <<endl; // For testing only

    // creates a standard deviation of the calibration squares for creating lower and upper thresholds
    Scalar mean_1, stddev_1;
    meanStdDev(Square_1,mean_1,stddev_1,cv::Mat());
    // cout<<"Standard deviation: "<<stddev_1.val[0]<<endl; // for testing only

    // Lower and upper thresholds. increased by 10% to create a wider detecble range. ~~~~ Needs further work ~~~~~
    Lower_Threshhold[0] = int( Square_1_mean[0] - (10*stddev_1.val[0]) );
    Lower_Threshhold[1] = int( Square_1_mean[1] - (10*stddev_1.val[0]) );
    Lower_Threshhold[2] = int( Square_1_mean[2] - (10*stddev_1.val[0]) );
    Upper_Threshhold[0] = int( Square_1_mean[0] + (10*stddev_1.val[0]) );
    Upper_Threshhold[1] = int( Square_1_mean[1] + (10*stddev_1.val[0]) );
    Upper_Threshhold[2] = int( Square_1_mean[2] + (10*stddev_1.val[0]) );

    // Simple Check to keep the range within 0 - 255
    for(int i = 0; i<3; i++)
    {
        if(Lower_Threshhold[i] > 255)
            Lower_Threshhold[i] = 255;
        if(Lower_Threshhold[i] < 0)
            Lower_Threshhold[i] = 0;
        if(Upper_Threshhold[i] > 255)
            Upper_Threshhold[i] = 255;
        if(Upper_Threshhold[i] < 0)
            Upper_Threshhold[i] = 0;
    }

    // Sets the slider to the value
    slider_LB = int (Lower_Threshhold[0]);
    slider_LG = int (Lower_Threshhold[1]);
    slider_LR = int (Lower_Threshhold[2]);
    slider_UB = int (Upper_Threshhold[0]);
    slider_UG = int (Upper_Threshhold[1]);
    slider_UR = int (Upper_Threshhold[2]);

    // Calls the slider function
    on_trackbar_Lower_Blue(slider_LB,0);
    on_trackbar_Lower_Green(slider_LG,0);
    on_trackbar_Lower_Red(slider_LR,0);
    on_trackbar_Upper_Blue(slider_UB,0);
    on_trackbar_Upper_Green(slider_UG,0);
    on_trackbar_Upper_Red(slider_UR,0);

    //  cout << "Lower_Threshhold" <<Lower_Threshhold <<endl; // For Testing Only
    //  cout << "Upper_Threshhold" <<Upper_Threshhold <<endl; // For Testing Only

    // Releases the data properly
    Square_1.release();
    Square_2.release();
}

void Draw_Contours(Mat &aThreshold_Applied, Mat &aLiveView, bool calibrationHappy)
{
    Mat Blurred_aTheshold;
    transmitGo = calibrationHappy;

    //morphological opening (remove small objects from the foreground)
    erode(aThreshold_Applied, Blurred_aTheshold, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)) );
    dilate( Blurred_aTheshold, Blurred_aTheshold, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)) );

    //morphological closing (fill small holes in the foreground)
    dilate( Blurred_aTheshold, Blurred_aTheshold, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)) );
    erode(Blurred_aTheshold, Blurred_aTheshold, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)) );

    //imshow("Eroded and Dilated",Blurred_aTheshold); // For Testing Only

    if(aThreshold_Applied.channels() == 1)
    {
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        findContours(Blurred_aTheshold, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);// CHAIN_APPROX_TC89_L1);

        if (contours.size() > 0) // finds largest contour
        {
            int biggest_contour_index = -1;
            double biggest_area = 0.0;

            for (int i = 0; i < contours.size(); i++)
            {
                double area = contourArea(contours[i], false);
                if (area > biggest_area)
                {
                    biggest_area = area;
                    biggest_contour_index = i;
                }
            }

            vector<vector<Point> > contours0;
            contours0.resize(contours.size() );
            if (biggest_contour_index >= 0)  // Draws the largest contour
            {
                contours.resize(contours0.size());
                for( size_t k = 0; k < contours0.size(); k++ )
                    approxPolyDP(Mat(contours[k]), contours0[k], 1, true);

                drawContours(aLiveView, contours0, biggest_contour_index, Scalar(0,255,0), 2, LINE_8, hierarchy, 0);

                /*
                 * // For Testing Only // Center of bounding circle
                 * float Radius;
                 * minEnclosingCircle(contours[biggest_contour_index], center, Radius);
                 * circle(aLiveView, center, int(Radius), Scalar(255,0,0), 2);
                 * circle(aLiveView, center, 5, Scalar(255,0,0), 3, LINE_8, 0);
                 */

                // Draws Bounding Rectangle abound hand
                // rectangle(aLiveView, minAreaRect(contours[biggest_contour_index]).boundingRect(), Scalar(0,0,255), 2); // For Testing Only
                // Finds Center of Bounding rectangle
                Rect_Center = minAreaRect(contours0[biggest_contour_index]).center;
                // Draws a "dot" on the center of the hand
                circle(aLiveView, Rect_Center, 5, Scalar(255,255,255), 3, LINE_8, 0);

                /*
                 * // For Testing Only // True center of contour
                 * float X(0.0);
                 * float Y(0.0);
                 * for(size_t k = 0; k <contours0[biggest_contour_index].size(); k++ )
                 * {
                 *  X+= contours0[biggest_contour_index][k].x;
                 *  Y+= contours0[biggest_contour_index][k].y;
                 * }
                 * X = X/contours0[biggest_contour_index].size();
                 * Y = Y/contours0[biggest_contour_index].size();
                 * True_Center = Point2f(X,Y);
                 * circle(aLiveView, True_Center, 5, Scalar(0,255,0), 3, LINE_8, 0);
                */
            }

            // ~~~~ Future Work required for this if statement ~~~~~ //
            if(true) // There should be a check here to reduce errors
            {
                vector<Point> hull_points;
                vector<int> hull_ints;

                convexHull(Mat(contours0[biggest_contour_index]), hull_points, true);
                convexHull(Mat(contours0[biggest_contour_index]), hull_ints, true);

                /* // For Testing Only
                 * Orginal method of finding center of hand but not used now
                 * Rect bounding_rectangle = boundingRect(Mat(hull_points));
                 * Point center_bounding_rect(
                 *                           (bounding_rectangle.tl().x + bounding_rectangle.br().x) / 2,
                 *                           (bounding_rectangle.tl().y + bounding_rectangle.br().y) / 2
                 *                           );
                 * circle(aLiveView, center_bounding_rect, 5, Scalar(255,255,255), 3, LINE_8, 0);
                 * rectangle(aLiveView, bounding_rectangle.tl(), bounding_rectangle.br(), Scalar(255,255,255), 2, 8, 0);
                 */

                vector<Vec4i> defects;

                if (hull_ints.size() > 3)       // Require at least 3 "ints" to find hand
                {
                    convexityDefects(Mat(contours0[biggest_contour_index]), hull_ints, defects);
                }

                vector<Point> start_points;    // Fingertips
                vector<Point> farthest_points; // Inter-digital folds

                // Loads Points into the vector
                for (int i = 0; i < defects.size(); i++)
                {
                    start_points.push_back(contours0[biggest_contour_index][defects[i].val[0]]);
                    farthest_points.push_back(contours0[biggest_contour_index][defects[i].val[2]]);
                }

                amend_start_pt = Amden_Points_Start(start_points, Rect_Center);
                amend_farthest_points = Amden_Points(farthest_points, Rect_Center);
                /*
                 * For Testing Only
                 * This section of the code places
                 * the "dots" on the screen
                 *
                 *
                 * for (int i = 0; i < amend_farthest_points.size(); i++)
                 *     {
                 *     circle(aLiveView, amend_farthest_points[i], 5, Scalar(255,0,0), 3, LINE_8, 0);
                 *     putText(aLiveView, to_string(i), amend_farthest_points[i], FONT_HERSHEY_PLAIN, 3, Scalar(255,0,0));
                 *     }
                 *
                 * for (int i = 0; i < amend_start_pt.size(); i++)
                 *    {
                 *    circle(aLiveView, amend_start_pt[i], 5, Scalar(0,0,255), 3, LINE_8, 0);
                 *    putText(aLiveView, to_string(i), amend_start_pt[i], FONT_HERSHEY_PLAIN, 3, Scalar(0,0,255));
                 *    }
                 *
                 *
                 * if(!amend_farthest_points.empty() )
                 *   {
                 *   vector<int> Defects_index;
                 *   for(int i =0; i<contours0[biggest_contour_index].size(); i++)
                 *      {
                 *      for(int j =0; j<amend_farthest_points.size(); j++)
                 *         {
                 *         if(contours0[biggest_contour_index][i] == amend_farthest_points[j])
                 *            Defects_index.push_back(i);
                 *         }
                 *       }
                 *
                 *   if(!Defects_index.empty() )
                 *     {
                 *     for(int j =0; j<Defects_index.size()-1; j++)
                 *        {
                 *        int half_way_point =  int ((Defects_index[j+1] - Defects_index[j])*0.5);
                 *        circle(aLiveView, contours0[biggest_contour_index][half_way_point] , 5, Scalar(0,255,255), 3, LINE_8, 0);
                 *        }
                 *     }
                 *   }
                 */

                if ((!amend_start_pt.empty() )&&(!amend_farthest_points.empty() ))
                {
                    assembleCommandWord( setSafeties(aThreshold_Applied, Rect_Center));
                    /*
                     * These display the angle of wrist rotation
                     */
                      circle(aLiveView, A, 10, Scalar(255,0,0), 3, LINE_8, 0); // For Testing Only
                      circle(aLiveView, B, 10, Scalar(0,0,0), 3, LINE_8, 0);   // For Testing Only
                      line(aLiveView, C, A, Scalar(0,0,0), 3, LINE_8, 0);      // For Testing Only
                      line(aLiveView, C, B, Scalar(0,0,0), 3, LINE_8, 0);      // For Testing Only

                }

                // putText(aLiveView, HandPose, Point(80,80), FONT_HERSHEY_PLAIN, 5, Scalar(0,0,255));   // Dispays handpose detection text on the screen
                // polylines(aLiveView, hull_points, true, Scalar(0,128,255), 2);                        // Draws lines around the hand allong the hull_points

                // realeses all data
                start_points.clear();
                farthest_points.clear();
                amend_start_pt.clear();
                amend_farthest_points.clear();
            }
        }
        contours.clear();
        hierarchy.clear();
    }
}

vector<Point> Amden_Points(vector<Point> &points, Point2f center)
{
    vector<double> Magnitude; // Magnitude of Points
    vector<Point> Amended;    // New Array of Points

    int r(20); // Radius of deletion
    int a(0);  // secondary counter for point reassingment

    // Checks to see if vector is empty
    if(!points.empty())
    {
        // Loads the first element for comparision
        Amended.push_back(points[0]);

        for(int i =1; i < points.size(); i++)
        {
            // Check if this is an internal point
            if (!((points[a].x - points[i].x)*(points[a].x - points[i].x) + (points[a].y - points[i].y)*(points[a].y - points[i].y) <= (r*r)))
            {
                // Loads the point if it is valid
                Amended.push_back(points[i]);
                // Uses the loaded point for the next comparison
                a=i;
            }
        }
        // Obtains the magnitude from center of the hand for each filtered point
        for(int i =0; i < Amended.size(); i++)
        {
            Magnitude.push_back(sqrt ( (Amended[i].x  - center.x) * (Amended[i].x  - center.x) + (Amended[i].y  - center.y) * (Amended[i].y  - center.y) ));
        }

        double temp(0);
        Point index_temp(0,0);
        // Bubble sort function for magnitude
        // Only concerned with the 5 smallest magnitudes
        for(int i =0; i< Amended.size(); i++)
        {
            for(int j =0; j< Amended.size();j++)
            {
                if(Magnitude[j] > Magnitude[i])
                {
                    temp = Magnitude[i];
                    Magnitude[i] = Magnitude[j];
                    Magnitude[j] = temp;

                    index_temp = Amended[i];
                    Amended[i] = Amended[j];
                    Amended[j] = index_temp;
                }
            }
        }
        // To ensure safety this should really be done with an iterator object ~~~ Needs to Change ~~~~~
        int first_5(5);
        // Checks to ensure the array is in range
        if(int (Amended.size()) < first_5)
        {
            first_5 = int (Amended.size());
        }
        Amended.resize(first_5);

        for(int i =0; i< Amended.size(); i++)
        {
            for(int j =0; j< Amended.size();j++)
            {
                if(Amended[j].x > Amended[i].x)
                {
                    index_temp = Amended[i];
                    Amended[i] = Amended[j];
                    Amended[j] = index_temp;
                }
            }
        }
    }
    return Amended;
}
// ~~~~ This function is the same as above except that is looks for Last 5 instead of first 5 ~~~~~
// ~~~~ Future work might see these function combined and return a vector<Point> (size 10) contain all desired points ~~~~
vector<Point> Amden_Points_Start(vector<Point> &points, Point2f center)
{
    vector<double> Magnitude; // Magnitude of Points
    vector<Point> Amended;    // New Array of Points

    int r(40); // Radius of deletion
    int a(0);  // secondary counter for point reassingment

    // Checks to see if vector is empty
    if(!points.empty())
    {
        //loads the first elment for comparision
        Amended.push_back(points[0]);

        for(int i =1; i < points.size(); i++)
        {
            // Check if this is an internal point
            if (!((points[a].x - points[i].x)*(points[a].x - points[i].x) + (points[a].y - points[i].y)*(points[a].y - points[i].y) <= (r*r)))
            {
                //loads the point as valid
                Amended.push_back(points[i]);
                // uses the loaded point for the next comparison
                a=i;
            }
        }

        // Obtains the magnitude from center of the hand for each filtered point
        for(int i =0; i < Amended.size(); i++)
        {
            Magnitude.push_back(sqrt ( (Amended[i].x  - center.x) * (Amended[i].x  - center.x) + (Amended[i].y  - center.y) * (Amended[i].y  - center.y) ));
        }

        double temp(0);
        Point index_temp(0,0);
        // Bubble sort function for magnitude only concerned with the 5 largest magnitudes
        for(int i =0; i< Amended.size(); i++)
        {
            for(int j =0; j< Amended.size();j++)
            {
                if(Magnitude[j] < Magnitude[i])
                {
                    temp = Magnitude[i];
                    Magnitude[i] = Magnitude[j];
                    Magnitude[j] = temp;

                    index_temp = Amended[i];
                    Amended[i] = Amended[j];
                    Amended[j] = index_temp;
                }
            }
        }
        int Last_5(5);
        //checks to ensure the array is in range
        if(int (Amended.size()) < Last_5)
        {
            Last_5 = int (Amended.size());
        }
        Amended.resize(Last_5);

        for(int i =0; i< Amended.size(); i++)
        {
            for(int j =0; j< Amended.size();j++)
            {
                if(Amended[j].x < Amended[i].x)
                {
                    index_temp = Amended[i];
                    Amended[i] = Amended[j];
                    Amended[j] = index_temp;
                }
            }
        }
    }
    return Amended;
}

void detectHandPose(char* HandPose)
{
    /*
     * A lot going on here:
     * First the magnitude of each amend point set is taken.
     * Then they are compared to eachother using a ratio of 1.86
     * **** See thesis why that ratio ****
     * Then a check is performed to ensure the last detection is defferent than the
     * current detection before transmission.
     */

    double averageMagintudeOfFigerTips = 0.0;
    double averageMagnitudeOfDefects   = 0.0;

    double Temp = 0.0;
    for(int i = 0; i< amend_start_pt.size(); i++)
    {
        Temp += sqrt ( (amend_start_pt[i].x  - Rect_Center.x) * (amend_start_pt[i].x  - Rect_Center.x) + (amend_start_pt[i].y  - Rect_Center.y) * (amend_start_pt[i].y  - Rect_Center.y) );
    }

    averageMagintudeOfFigerTips = Temp / ((double) amend_start_pt.size());

    Temp = 0.0;
    for(int i = 0; i< amend_farthest_points.size(); i++)
    {
        Temp += sqrt ( (amend_farthest_points[i].x  - Rect_Center.x) * (amend_farthest_points[i].x  - Rect_Center.x) + (amend_farthest_points[i].y  - Rect_Center.y) * (amend_farthest_points[i].y  - Rect_Center.y) );
    }

    averageMagnitudeOfDefects = Temp / ((double) amend_farthest_points.size());

    // True is Open hand
    // False is closed fist
    static bool LastValue = false;
    static char* LastHandPose= "F00P00"
                               "F02P00"
                               "F03P00"
                               "F04P00"
                               "F05P00";

    if((averageMagintudeOfFigerTips > (1.86 * averageMagnitudeOfDefects) ) && (LastValue == false))
    {
        // cout << "Open Hand Detected" << endl;                                                 // For Testing only
        // cout << "Average Magintude Of Figer Tips: " << averageMagintudeOfFigerTips << endl;   // For Testing only
        // cout << "Average Magintude Of Defects: " << averageMagnitudeOfDefects << endl;        // For Testing only
        LastValue = true;
        LastHandPose = FormatMovement("stop");

        for(int i=0; LastHandPose[i]!='\0';i++)
        {
            HandPose[i] = LastHandPose[i];
        }
        // HandPose = "Open Hand"; // For Screen Printing
    }
    else if((averageMagintudeOfFigerTips < (1.86 * averageMagnitudeOfDefects)) && (LastValue == true))
    {
        // cout << "Closed Hand Detected" << endl;                                              // For Testing only
        // cout << "Average Magintude Of Figer Tips: " << averageMagintudeOfFigerTips << endl;  // For Testing only
        // cout << "Average Magintude Of Defects: " << averageMagnitudeOfDefects << endl;       // For Testing only
        LastValue = false;
        LastHandPose = FormatMovement("fist");

        for(int i=0; LastHandPose[i]!='\0';i++)
        {
            HandPose[i] = LastHandPose[i];
        }
        // HandPose = "Closed Hand"; // For Screen Printing
    }
    else // keeps the hand position static while no detection is made
    {
        for(int i=0; LastHandPose[i]!='\0';i++)
        {
            HandPose[i] = LastHandPose[i];
        }
    }
}

void drawAxisAndQuadents(Mat &aLive_View)
{
    //X-Axis
    line(aLive_View,
         Point(aLive_View.size().width, int (aLive_View.size().height * 0.5) ),
         Point(0,int (aLive_View.size().height * 0.5) ),
         Scalar( 255, 0, 0 ),
         2,
         LINE_8);
    //Y-Axis
    line(aLive_View,
         Point(int (aLive_View.size().width * 0.5), aLive_View.size().height ),
         Point(int (aLive_View.size().width * 0.5), 0 ),
         Scalar( 255, 0, 0 ),
         2,
         LINE_8);
    //Y-Upper Bounds
    line(aLive_View,
         Point(aLive_View.size().width, int (aLive_View.size().height * 0.3) ),
         Point(0,int (aLive_View.size().height * 0.3) ),
         Scalar( 0, 255, 0 ),
         2,
         LINE_8);
    //Y-Lower Bounds
    line(aLive_View,
         Point(aLive_View.size().width, int (aLive_View.size().height * 0.7) ),
         Point(0,int (aLive_View.size().height * 0.7) ),
         Scalar( 255, 255, 255 ),
         2,
         LINE_8);
    //X Left Bounds
    line(aLive_View,
         Point(int (aLive_View.size().width * 0.3), aLive_View.size().height ),
         Point(int (aLive_View.size().width * 0.3), 0 ),
         Scalar( 0, 255, 255 ),
         2,
         LINE_8);
    //X Right Bounds
    line(aLive_View,
         Point(int (aLive_View.size().width * 0.7), aLive_View.size().height ),
         Point(int (aLive_View.size().width * 0.7), 0 ),
         Scalar( 0, 255, 0 ),
         2,
         LINE_8);
}

int setSafeties(Mat &aThreshold_Applied, Point2f center)
{
    /*
    * Several checks are happening here
    * *** Note *** the "crossProduct" function returns a "1" or "0"
    * The screen is split into 12 regions
    * The first two tests narrow down the hands location on the screen
    * to 1 of 4 locations. The second two tests find out which of the
    * 4 regions the hand is loacated in.
    */

    int aboveXAxis = crossProduct(Point(aThreshold_Applied.size().width, int (aThreshold_Applied.size().height * 0.5)),
                                  Point(0,int (aThreshold_Applied.size().height * 0.5)),
                                  center);

    int rightYAxis =  crossProduct(Point(int (aThreshold_Applied.size().width * 0.5), aThreshold_Applied.size().height ),
                                   Point(int (aThreshold_Applied.size().width * 0.5), 0 ),
                                   center);
    double YBounds = 0.3; // Precentage of the screen
    double XBounds = 0.7; // Precentage of the screen
    if(aboveXAxis == 1 )
    {
        XBounds = 0.3;
    }
    if(rightYAxis == 1 )
    {
        YBounds = 0.7;
    }

    /*
     * The Boundy is either 30% of 70%
     * Based upon the first two test
     */
    int XAxisBoundry = crossProduct(Point(aThreshold_Applied.size().width, int (aThreshold_Applied.size().height * XBounds)),
                                    Point(0,int (aThreshold_Applied.size().height * XBounds)),
                                    center);

    int YAxisBoundry = crossProduct(Point(int (aThreshold_Applied.size().width * YBounds), aThreshold_Applied.size().height ),
                                    Point(int (aThreshold_Applied.size().width * YBounds), 0 ),
                                    center);

    int quadrantControl(0); // Needs to be intiated

    /*
     * Each test returns a "1" or "0"
     * compining the all the "1's" and "0's" into
     * a four bit binary number creates a decimal number
     * 0 - 15 which is used as an array index for setting safeties
     */
    quadrantControl |= YAxisBoundry |(XAxisBoundry << 1)|(rightYAxis << 2)|(aboveXAxis << 3);

    /*
     * The following table shows the number
     * asscoiated with the quadrants
     *  ______________________
     *  | 10  | 11 | 14 | 15 |
     *  ----------------------
     *  |  8  |  9 | 12 | 13 |
     *  ----------------------
     *  |  2  |  3 |  6 |  7 |
     *  ----------------------
     *  |  0  |  1 |  4 |  5 |
     *  ----------------------
     */

    //cout<<quadrantControl<<endl;

    // ~~~~ Future work will see this and the return statement simplifiedd ~~~~ //
    EnabledMovements = DirectionalMovements[quadrantControl];
    return ( EnableMovements[quadrantControl] );
}

void assembleCommandWord(int safetyByte)
{
    char CommandWord[50]; //Holds all the commands for all servo motors

    // static is chosen here to ensure the hand postition
    // is kept when not detect

    static char HandPose[31] = {"F00P00"   // Pinky
                                "F02P00"   // Ring
                                "F03P00"   // Middle
                                "F04P00"   // Index
                                "F05P00"}; // Thumb

    char Bicep[7]    = {"B99P99"}; // "99" does not exsits and will not cause an error
    char Shoulder[7] = {"S99P99"};
    char Wrist[7]    = {"W99P99"};

    /*
     * Simplified switch statment to control
     * different motors as desired
     */

    switch(safetyByte)
    {
    case 1:
        StopBicep(Bicep);
        getShoulder(Shoulder);
        StopWrist(Wrist);
        break;
    case 2:
        getBicep(Bicep);
        StopShoulder(Shoulder);
        StopWrist(Wrist);
        break;
    case 3:
        getBicep(Bicep);
        getShoulder(Shoulder);
        StopWrist(Wrist);
        break;
    case 4:
        detectHandPose(HandPose);
        getWrist(Wrist);
        StopBicep(Bicep);
        StopShoulder(Shoulder);
        break;
    }

    // Self built array coping function
    ConcentrateCharArrays(CommandWord, HandPose, Wrist, Bicep, Shoulder );

    //cout<<"CommandWord: " << CommandWord << endl; // For Testing Only

    /*
     * The "transmit" function should be commented out
     * when running just camera test as well as "socketSetUp"
     * funciton in the "main.cpp" file
     */
    if(transmitGo ==true)
    {
    transmit(CommandWord);
    }
}

void getBicep(char* Bicep)
{
    char* Movement;

    switch(EnabledMovements)
    {
    case 1:
        Movement = "B00P12";//Curl
        break;
    case 2:
        Movement = "B00P13";//Extend
        break;
    case 5:
        Movement = "B00P12";//Curl
        break;
    case 6:
        Movement = "B00P12";//Curl
        break;
    case 7:
        Movement = "B00P13";//Extend
        break;
    case 8:
        Movement = "B00P13";//Extend
        break;
    }

    for(int i=0; Movement[i]!='\0';i++)
    {
        Bicep[i] = Movement[i];
    }
}
void getShoulder(char* Shoulder)
{
    char* Movement;
    switch(EnabledMovements)
    {
    case 3:
        Movement = "S00P11";//Left
        break;
    case 4:
        Movement = "S00P13";//Right
        break;
    case 5:
        Movement = "S00P11";//Left
        break;
    case 6:
        Movement = "S00P13";//Right
        break;
    case 7:
        Movement = "S00P11";//Left
        break;
    case 8:
        Movement = "S00P13";//Right
        break;
    }

    for(int i=0; Movement[i]!='\0';i++)
    {
        Shoulder[i] = Movement[i];
    }
}

//~~~~~~~~~ NEEDS WORK ~~~~~~~~~//
void getWrist(char* Wrist)
{
    char* Movement ;

    //Law of Cosine https://www.mathsisfun.com/algebra/trig-cosine-law.html

    if(Rect_Center.y - 100 > 0)
    {
        A = Point2f(Rect_Center.x, Rect_Center.y - 100);
    }
    else //Always going to be here... left in for safety // could be removed to increase speed
    {
        A = Point2f(Rect_Center.x, Rect_Center.y + 100);
    }

    AverageFingerTips();
    B = RunningAverageWristAngle;
    C = Rect_Center;

    //cout<< "Point A: " << A <<endl;
    //cout<< "Point B: " << B <<endl;
    //cout<< "Point C: " << C <<endl;

    double a = Magnitude(B, C); //Find length of BC
    double b = Magnitude(A, C); //Find length of AC
    double c = Magnitude(A, B); //Find length of AB

    double Angle = acos(((a*a)+(b*b)-(c*c))/(2*a*b));
    Angle = Angle*180 / 3.141529;
    cout<< "Angle: " << Angle <<endl;


  if(Angle > 50.0)
  {
       static char AngleCommand;
       if( B.x > Rect_Center.x )
       {
        Movement = "W00P05"; // Left
       }
       else
       {
        Movement = "W00P05"; // Right
       }
  }
  else
  {
      Movement = "W00P25"; // Center
  }

    // cout <<"Movement: " << Movement<< P << endl; // For Testing Only

    for(int i=0; Wrist[i]!='\0';i++)
    {
        Wrist[i] = Movement[i];
    }
}

void StopBicep(char* Bicep)
{
    char Stop[7] {"B00P00"};

    for(int i=0; Bicep[i]!='\0';i++)
    {
        Bicep[i] = Stop[i];
    }
}
void StopWrist(char* Wrist)
{
    char Stop[7] {"W00P00"};

    for(int i=0; Wrist[i]!='\0';i++)
    {
        Wrist[i] = Stop[i];
    }
}
void StopShoulder(char* Shoulder)
{
    char Stop[7] {"S00P00"};

    for(int i=0; Shoulder[i]!='\0';i++)
    {
        Shoulder[i] = Stop[i];
    }
}
void ConcentrateCharArrays(char* CommandWord, char* Handpose, char* Wrist, char* Bicep, char* Shoulder)
{
    int counter = 0;

    for(int i=0; Handpose[i]!='\0';i++)
    {
        CommandWord[counter] = Handpose[i];
        counter++;
    }
    for(int i=0; Wrist[i]!='\0';i++)
    {
        CommandWord[counter] = Wrist[i];
        counter++;
    }
    for(int i=0; Bicep[i]!='\0';i++)
    {
        CommandWord[counter] = Bicep[i];
        counter++;
    }
    for(int i=0; Shoulder[i]!='\0';i++)
    {
        CommandWord[counter] = Shoulder[i];
        counter++;
    }
    CommandWord[counter] = '\0';
}
int crossProduct(Point2f startPoint, Point2f EndPoint, Point2f Compare)
{
    double VectorOneX = EndPoint.x - startPoint.x;
    double VectorOneY = EndPoint.y - startPoint.y;

    double VectorTwoX = EndPoint.x - Compare.x;
    double VectorTwoY = EndPoint.y - Compare.y;

    double cross = (VectorOneX * VectorTwoY) - (VectorOneY * VectorTwoX) ;

    /*
     * Top left corner is [0,0]
     * Bottom right is [Width, Height]
     */
    if(cross > 0.0)
    {
        //Below or left of the line
        return (0);
    }
    else
    {
        //Above or right of the line
        return  (1);
    }
}
void AverageFingerTips()
{
    float x = 0.0;
    float y = 0.0;

    // Static variables allow function variable to be
    // remebered when returning to the function
    static vector<Point2f> AverageFingers;
    static int counter = 0;
    static Point2f Last_Rect_Center(0.0,0.0);
   /*
    * If the center of the hand moves then then the
    * wrist rottation can be effected
    * to counter this the running average must be reset if the
    * cente of the hand moves dramatical
    * radius of 20 pixels
    */

    double centerhandcheck = Magnitude(Rect_Center, Last_Rect_Center);
    if(centerhandcheck > 20.0)
    {
        AverageFingers.clear();
    }

    Last_Rect_Center = Rect_Center;

    for (int i = 0; i < amend_start_pt.size(); i++)
    {
        x += amend_start_pt[i].x;
        y += amend_start_pt[i].y;
    }
    x = x/amend_start_pt.size();
    y = y/amend_start_pt.size();

    Point2f TempAverageFingers = Point2f(x,y); // Average value of all fingertips

    if(AverageFingers.size() == 10) // Has the vector reached is max size?
    {
        AverageFingers[counter] = TempAverageFingers; // Add the Averaged value of fingertips to a vector
    }
    else
    {
        AverageFingers.push_back(TempAverageFingers); // creates new elements for the vector
    }
    counter++;
    if(counter==10) // reset the location to overwrite the vector with the oldest data
    {
        counter = 0;
    }

    Point2f Average;

    // Averages the Vector of averaged fingertips to a single point
    for( int i =0; i< AverageFingers.size(); i++ )
    {
        Average.x = Average.x + AverageFingers[i].x;
        Average.y = Average.y + AverageFingers[i].y;
    }

    Average.x = Average.x/ (float)AverageFingers.size();
    Average.y = Average.y/ (float)AverageFingers.size();
    // ~~~~ Future work have this function have a return instead of setting a global variable ~~~~ //
    RunningAverageWristAngle = Average;
}

double Magnitude(Point2f aPoint, Point2f bPoint)
{
    // Standard Magnitude function
    // See website for more explination
    // https://www.varsitytutors.com/hotmath/hotmath_help/topics/magnitude-and-direction-of-vectors
    return ((double) sqrt( ((bPoint.x-aPoint.x)*(bPoint.x-aPoint.x)) + (bPoint.y-aPoint.y)*(bPoint.y-aPoint.y)));
}
