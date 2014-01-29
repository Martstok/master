#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <string>

class Image;
class GuiParameters;
class HoughLine;

class HoughTransform
{
private:
    void drawSingleLongHoughLine(cv::Mat dst, HoughLine* houghLine);
public:
    std::vector<HoughLine*> longLines;
    std::vector<cv::Vec4i> shortLines;
    std::vector<int> test;
    std::vector<HoughLine*> verticalLines;
    std::vector<std::vector<HoughLine*> > lineGroups;

    HoughTransform();
    void generateLongHoughLines(Image* img, GuiParameters* guiParameters);
    void generateShortHoughLines(Image* img, GuiParameters* guiParameters);
    void drawLongHoughLines(Image* img, GuiParameters* guiParameters);
    void drawShortHoughLines(Image* img, GuiParameters* guiParameters);

    void extractVerticalLines(Image* img, int frameRollBias, int tolerance);
    void groupLinesByAngle();
    void sortLinesByAngle();
};

#endif // HOUGHTRANSFORM_H
