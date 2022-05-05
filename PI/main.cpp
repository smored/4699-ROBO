#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#include "robot.h"

using namespace std;

int main(int argc, char* argv[]) {
    robot robotRun = robot();
    robotRun.runLoop();

}
