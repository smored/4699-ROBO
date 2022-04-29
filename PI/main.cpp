#include <opencv2/opencv.hpp>
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define FRAME_HEIGHT 300 // for video capture resolution

using namespace std;

cv::VideoCapture _video;
cv::Mat _canvas;

int main(int argc, char* argv[]) {
//bool worked = false;

    // open video capture
   /* for (int i=-1; i<255; i++) {
    worked = true;
        try{
            _video.open(i, cv::CAP_ANY);
        } catch(exception e) {
            std::cerr << "bad" << std::endl;
            worked = false;
        }
        if (worked) {
            std::cout << "worked " << std::to_string(i) << std::endl;
        }
    }*/

    _video.open(0, cv::CAP_ANY);

    const cv::Size canvasSize = cv::Size(FRAME_HEIGHT*16/9, FRAME_HEIGHT);
    _video.set(cv::CAP_PROP_FRAME_WIDTH, canvasSize.width);
    _video.set(cv::CAP_PROP_FRAME_HEIGHT, canvasSize.height);

    _canvas = cv::Mat::zeros(canvasSize, CV_8UC3);
    cv::imshow("RGB", _canvas);

    while(1) {
    // put frame on canvas
    _video >> _canvas;
    }

}
