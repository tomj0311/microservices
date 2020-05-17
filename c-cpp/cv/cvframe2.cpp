#include "opencv2/opencv.hpp"
#include <time.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{

    // Start default camera
    VideoCapture video(0);
    
    // With webcam get(CV_CAP_PROP_FPS) does not work.
    // Let's see for ourselves.
    
    double fps = video.get(CAP_PROP_FPS);
    // If you do not care about backward compatibility
    // You can use the following instead for OpenCV 3
    // double fps = video.get(CAP_PROP_FPS);
    cout << "Frames per second using video.get(CV_CAP_PROP_FPS) : " << fps << endl;

    video.set(CAP_PROP_FOURCC, 6);
    video.set(CAP_PROP_FRAME_WIDTH, 1280);
    video.set(CAP_PROP_FRAME_HEIGHT, 720);
    

    // Number of frames to capture
    int num_frames = 480;
    
    // Start and end times
    time_t start, end;
    
    // Variable for storing video frames
    Mat frame;

    cout << "Capturing " << num_frames << " frames" << endl ;

    // Start time
    time(&start);
    
    // Grab a few frames
    for(int i = 0; i < num_frames; i++)
    {
        video >> frame;
        imshow("Frame window", frame);

        char c = (char)waitKey(1);
        if( c == 27 ) 
            break;
    }
    
    // End Time
    time(&end);
    
    // Time elapsed
    double seconds = difftime (end, start);
    cout << "Time taken : " << seconds << " seconds" << endl;
    
    // Calculate frames per second
    fps  = num_frames / seconds;
    cout << "Estimated frames per second : " << fps << endl;
    
    // Release video
    video.release();
    return 0;
}