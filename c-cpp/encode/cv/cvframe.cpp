#include <iostream>
#include <ctime>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

int main()
{
    cv::VideoCapture cam;

    if (!cam.open(0))
        std::cout << "Problem connecting to cam " << std::endl;
    else
        std::cout << "Successfuly connected to camera " << std::endl;

    long frameCounter = 0;

    std::time_t timeBegin = std::time(0);
    int tick = 0;

    cv::Mat frame;

    int key = 1;
    while (key != 27)
    {
        cam.read(frame);
        cv::imshow("Img", frame);

        frameCounter++;

        std::time_t timeNow = std::time(0) - timeBegin;

        if (timeNow - tick >= 1)
        {
            tick++;
            cout << "Frames per second: " << frameCounter << endl;
            frameCounter = 0;
        }

        key = cv::waitKey(1);
    }

    return 0;
}