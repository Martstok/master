#ifndef HOUGHLINE_H
#define HOUGHLINE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <string>

class HoughLine
{
public:
    cv::Vec2f rawLine;
    int angle;

    HoughLine();
    HoughLine(cv::Vec2f rawLine);
    int calculateAngle(cv::Vec2f rawLine);

};

#endif // HOUGHLINE_H
