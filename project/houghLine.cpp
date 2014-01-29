#include "houghLine.h"

HoughLine::HoughLine()
{
}

HoughLine::HoughLine(cv::Vec2f rawLine){
    this->rawLine = rawLine;
    this->angle = calculateAngle(rawLine);

}

int HoughLine::calculateAngle(cv::Vec2f rawLine){
    float rho = this->rawLine[0];
    int sgn_rho = (rho>0)-(rho<0);
    double deg = (-this->rawLine[1]+CV_PI/2);
    double angle = deg/CV_PI*180+90;
    return angle;
}
