#include "houghTransform.h"
#include "image.h"
#include "gui.h"
#include <iostream>
#include "houghLine.h"

using namespace std;
using namespace cv;

HoughTransform::HoughTransform()
{
}

//Canny transform, then hough transform
//void HoughTransform::generateLongHoughLines(Image* img, GuiParameters* guiParameters){
//    img->rawLR.copyTo(img->canny);
//    Canny(img->rawLR, img->canny, 50, 200, 3);
//    HoughLines(img->canny, this->longLines, 1, CV_PI/180, guiParameters->houghThreshold+1);
//}

void HoughTransform::generateLongHoughLines(Image* img, GuiParameters* guiParameters){
    img->rawLR.copyTo(img->canny);
    Canny(img->rawLR, img->canny, 50, 200, 3);

    vector<Vec2f> lines;
    HoughLines(img->canny, lines, 1, CV_PI/180, guiParameters->houghThreshold+1);
    for(int i = 0; i < lines.size(); i++){
//        Find angle
        HoughLine* houghLine = new HoughLine(lines[i]);
        this->longLines.push_back(houghLine);

    }
}


void HoughTransform::generateShortHoughLines(Image* img, GuiParameters* guiParameters){
    HoughLinesP(img->canny, this->shortLines, 1, CV_PI/180, guiParameters->houghThreshold+1, guiParameters->houghMinLength, guiParameters->houghMaxGap );

}


void HoughTransform::drawLongHoughLines(Image* img, GuiParameters* guiParameters){
    img->rawLR.copyTo(img->hough);
    img->rawLR.copyTo(img->result);

    for( size_t i = 0; i < this->longLines.size(); i++ )
    {
//      Draw each houghLine
        drawSingleLongHoughLine(img->hough, this->longLines[i]);

      //Plot parameters for all lines
        circle( img->houghPlot, Point(abs(this->longLines[i]->rawLine[0]), this->longLines[i]->angle), 4, Scalar(255), 2);
    }

}
void HoughTransform::drawSingleLongHoughLine(Mat dst, HoughLine* houghLine){
    float rho = houghLine->rawLine[0], theta = houghLine->rawLine[1];
    Point pt1, pt2;
    double a = cos(theta), b = sin(theta);
    double x0 = a*rho, y0 = b*rho;
    pt1.x = cvRound(x0 + 1000*(-b));
    pt1.y = cvRound(y0 + 1000*(a));
    pt2.x = cvRound(x0 - 1000*(-b));
    pt2.y = cvRound(y0 - 1000*(a));
    line(dst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
//    cout << abs(int(rho)) << ", " << houghLine[1]/CV_PI*180;  //Print line parameters
}


void HoughTransform::drawShortHoughLines(Image* img, GuiParameters* guiParameters){
    img->rawLR.copyTo(img->result2);
    for( size_t i = 0; i < this->shortLines.size(); i++ )
    {
      Vec4i l = this->shortLines[i];
      line( img->result2, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 3, CV_AA);

    }
    cout << endl << "-------------------" << endl;

}


void HoughTransform::extractVerticalLines(Image* img, int frameRollBias, int tolerance){
    // Find vertical lines
    vector<Vec2f> verticalLines;
    for(int i = 0; i < this->longLines.size(); i++){
        double deg = (-this->longLines[i]->rawLine[1]+CV_PI/2);
        double angle = deg/CV_PI*180+90;
        if(angle < frameRollBias + tolerance || angle > 180+frameRollBias-tolerance){
            this->verticalLines.push_back(this->longLines[i]);
//            cout << "Angle: " << angle << endl;

        }

    }

    // Draw vertical lines
    img->rawLR.copyTo(img->result);
    for(int i = 0; i<this->verticalLines.size(); i++){
        drawSingleLongHoughLine(img->result, this->verticalLines[i]);
    }
}

void HoughTransform::groupLinesByAngle(){
    sortLinesByAngle();
    int tolerance = 3;
    vector<HoughLine*> newGroup;
    for(int i = 0; i < this->longLines.size(); i++){
        if(this->longLines[i]->angle > newGroup.back()->angle + tolerance){
            this->lineGroups.push_back(newGroup);
            newGroup.clear();
            newGroup.push_back(this->longLines[i]);
        }
        else{
            newGroup.push_back(this->longLines[i]);

        }
    }
    this->lineGroups.push_back(newGroup);
}

void HoughTransform::sortLinesByAngle(){
    if (this->longLines.empty()){
        return;
    }
    vector<HoughLine*> sortedLines;
    sortedLines.push_back(this->longLines[0]);
    for(int i = 1; i < this->longLines.size(); i++){
        vector<HoughLine*>::iterator it;
        for(it = sortedLines.begin(); it != sortedLines.end(); it++){
            if(this->longLines[i]->angle < (*it)->angle){
                sortedLines.insert(it, this->longLines[i]);
                break;
            }
        }
    }
    this->longLines = sortedLines;
}


