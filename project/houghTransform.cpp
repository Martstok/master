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

HoughTransform::~HoughTransform(){
    for(int i = 0; i < this->longLines.size(); i++){
        delete this->longLines[i];
    }
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
    bool mergeFirstAndLast = false;
    sortLinesByAngle();
    this->lineGroups.clear();
    int tolerance = 3;
    vector<HoughLine*> newGroup;
    for(int i = 0; i < this->longLines.size(); i++){
        cout << "Examining: " << this->longLines[i]->angle << endl;


//        Create new group if previous angle differs by more than tolerance
        if(!newGroup.empty() && (this->longLines[i]->angle > newGroup.back()->angle + tolerance)){
            this->lineGroups.push_back(newGroup);
            newGroup.clear();
            newGroup.push_back(this->longLines[i]);
        }

//        Continue in same group previous angle differs by <= tolerance
        else{
            newGroup.push_back(this->longLines[i]);

        }

//        Merge last group with first if belong together
        cout << "begin merge" << endl;
        bool a = (i+1 == this->longLines.size());
        cout << "---" << endl;
//        int b = this->lineGroups.front().front()->angle;
        cout << "---2" << endl;
        int c = newGroup.back()->angle;
        cout << "---3" << endl;
        cout << "i = " << i << endl;
        if(i+1 == this->longLines.size() && (i != 0) && (!this->lineGroups.empty()) && (this->lineGroups.front().front()->angle - (newGroup.back()->angle - 180))<tolerance){
            cout << "check if groups is empty" << endl;
                mergeFirstAndLast = true;
                cout << "initiating loop" << endl;
                for(int i = newGroup.size()-1; i>=0; i--){
                    this->lineGroups[0].insert(this->lineGroups[0].begin(), newGroup[i]);
            }
        }
        cout << "end merge" << endl;
    }
    if(!mergeFirstAndLast){
        this->lineGroups.push_back(newGroup);
    }

    for(int i = 0; i < this->lineGroups.size(); i++){
        cout << "New group ------------------" << endl;
        for(int j = 0; j < this->lineGroups[i].size(); j++){
//            cout << "Angle: " << (this->lineGroups[i][j])->angle << endl;
        }
    }
    cout << "nr of groups: " << this->lineGroups.size() << endl;
    newGroup.clear();
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
        if(it == sortedLines.end()){
            sortedLines.push_back(this->longLines[i]);
        }
    }
    this->longLines = sortedLines;
}


