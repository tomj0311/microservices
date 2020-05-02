#include "cvframe3.hpp"

using namespace std;
using namespace cv;

MyCap * MyCap::s_instance = NULL;

MyCap* MyCap::instance() {
    if (s_instance == NULL)
        s_instance = new MyCap();
    return s_instance;
}

void MyCap::RunThreads(MyCap * cap) {
    cap->run();
}

void MyCap::run() {
    namedWindow("flow", 1);
    cap.open(0);

    // Inside loop.
    while (keepRunning) {
        cap >> frame;
        imshow("flow", frame);
        if (waitKey(1) >= 0) {
            break;
        }
    }
}

int main(int argc, char** argv) {
    auto myCap = MyCap::instance();
    myCap->capThreads.push_back(thread(&MyCap::RunThreads, MyCap::instance()));

    for (int i = 0; i < MyCap::instance()->capThreads.size(); i++) {
        cout << MyCap::instance()->capThreads.size();
        MyCap::instance()->capThreads[i].join();
    }
}