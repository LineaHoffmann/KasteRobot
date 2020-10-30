#include "ximagehandler.hpp"

ximageHandler::ximageHandler()
{

}

ximageHandler::ximageHandler(cv::Mat cleanImg)
{
    loadImage(cleanImg);

    if (cutOutTable()){
        std::cout << "succesfull ROI" << std::endl;
    } else {
        std::cout << "failed ROI" << std::endl;
    }
}

void ximageHandler::loadImage(cv::Mat image)
{
    image.copyTo(inputImage);
}

bool ximageHandler::cutOutTable()
{
    cv::Mat grey;
    if (debug) cv::imshow( "input", inputImage);
    cv::cvtColor( inputImage, grey, cv::COLOR_BGR2GRAY ); //Convert to gray
    cv::Mat kernell(5,5, CV_8U, cv::Scalar(1));
    cv::morphologyEx(grey, grey, cv::MORPH_GRADIENT, kernell); //det smart
    if (debug) cv::imshow( "closedimggrey", grey);

    cv::threshold(grey, grey, 35, 255, cv::THRESH_BINARY_INV);

    //remove noice
    cv::Mat kernel (2, 2, CV_8U, cv::Scalar(1));
    cv::dilate(grey, grey, kernel);
    cv::erode(grey, grey, kernel);
    cv::medianBlur(grey, grey, 5);
    cv::bitwise_not ( grey, grey );
    if (debug) cv::imshow( "filtering and convert to bin", grey);

    // find contours. Makes it easier to sort for what we want.
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(grey,contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //remove small contours, mostly all the cirles on the table.
    for (std::vector<std::vector<cv::Point> >::iterator it = contours.begin(); it!=contours.end(); )
    {
        if (it->size()<150)
            it=contours.erase(it);
        else

            ++it;
    }

    //draw filled contours of whats left.
    cv::Mat drawing = cv::Mat::zeros( grey.size(), CV_8UC3 );
    for(unsigned int i = 0; i< contours.size(); i++ )
    {
        cv::drawContours(drawing, contours, i, cv::Scalar(255, 255, 255), cv::FILLED);
    }
    //close egdes.
    cv::morphologyEx(drawing, drawing, cv::MORPH_CLOSE, kernell);
    cv::cvtColor(drawing, grey, cv::COLOR_BGR2GRAY);

    //find corners of table
    int lastCount = 0;
    int thisCount;
    int offset = 2;
    cv::Point topLeftCorner, topRightCorner, botLeftCorner, botRightCorner;

    for (int i = 0; i < grey.cols; i++){
        thisCount = 0;
        topLeftCorner = cv::Point(i,0);

        for (int j = 0; j < grey.rows; j++){
            if (grey.at<uchar>(j,i) >= 120) {
                thisCount++;
                botLeftCorner = cv::Point(i,j);
            }

        }
        if(thisCount > offset*offset*10 && thisCount - lastCount < offset) {
            break;
        }
        lastCount = thisCount;
    }
    for (int i = grey.cols; i > 0; i--){
        thisCount = 0;
        topRightCorner = cv::Point(i,0);

        for (int j = 0; j < grey.rows; j++){
            if (grey.at<uchar>(j,i) >= 120) {
                thisCount++;
                botRightCorner = cv::Point(i,j);
            }

        }
        if(thisCount > offset*offset*10 && thisCount - lastCount < offset) {
            break;
        }
        lastCount = thisCount;
    }

    cv::line(drawing, topLeftCorner, botLeftCorner, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
    cv::line(drawing, topRightCorner, botRightCorner, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);

    if (debug) cv::imshow("where it found table egdes", drawing);




    // Crop the full image to that image contained by the rectangle myROI
    // Note that this doesn't copy the data
    ROI = cv::Rect(topLeftCorner, botRightCorner);
    table = inputImage(ROI);
    if (table.cols - inputImage.cols < 0){
        //pixel to cm hackerway
        pixToCm = table.cols / tableWidth;

        if (showResult) cv::imshow("table", table);
        return 1;
    } else {
        return 0;
    }
}

std::pair<cv::Point2f, float> ximageHandler::findBallAndPosition(cv::Mat image)
{
    loadImage(image);
    if(dectectBall()) {
        return std::pair<cv::Point2f, float>(getPositionCM(),getRadiusCM());

    } else {
        return std::pair<cv::Point2f, float>(cv::Point2f(), 0);
    }
}


void ximageHandler::ballColor(int hue, int spread)
{
    //std::vector<int> colorRange = {0,30,170,180};
    colorRange.clear();
    if (hue-spread > 0){
        colorRange.push_back(0);
        colorRange.push_back(hue+spread);
        colorRange.push_back(180-(hue-spread));
        colorRange.push_back(180);

    } else {
        colorRange.push_back(hue-spread);
        colorRange.push_back(hue);
        colorRange.push_back(hue+1);
        colorRange.push_back(hue+spread);
    }
}

bool ximageHandler::dectectBall()
{
    cv::Mat hsv_image;
    cv::cvtColor(table, hsv_image, cv::COLOR_BGR2HLS);


    // Threshold the HSV image, keep only the red pixels
    cv::Mat lower_red_hue_range;
    cv::Mat upper_red_hue_range;
    cv::Mat bin;
    cv::inRange(hsv_image, cv::Scalar(colorRange[0], 100, 200), cv::Scalar(colorRange[1], 240, 255), lower_red_hue_range);
    cv::inRange(hsv_image, cv::Scalar(colorRange[2], 100, 200), cv::Scalar(colorRange[3], 240, 255), upper_red_hue_range);
    cv::addWeighted(lower_red_hue_range, 1, upper_red_hue_range, 1, 0, bin);

    //remove noice
    cv::Mat kernel (3, 3, CV_8U, cv::Scalar(1));

    cv::erode(bin, bin, kernel);
    cv::dilate(bin, bin, kernel);
    cv::medianBlur(bin, bin, 7);

    //find contours
    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(bin,contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //remove small contours
    for (std::vector<std::vector<cv::Point> >::iterator it = contours.begin(); it!=contours.end(); )
    {
        if (it->size()<(20)) {
            it=contours.erase(it);
        } else {
            ++it;
        }
    }

    cv::Point2f thisCenterPixel;
    float thisRadius;
    //draw filled contours of whats left.
    cv::Mat drawing = table;
    for( int i = 0; i< contours.size(); i++ )
    {
        cv::minEnclosingCircle(contours[i], thisCenterPixel, thisRadius);

        if (showResult) cv::circle(drawing, thisCenterPixel, thisRadius, cv::Scalar(0, 255, 0), 2 );
        if (showResult) cv::circle(drawing, robotBase, 85, cv::Scalar(255, 255, 255), 2 );
        if (showResult) cv::imshow("ball", drawing);
        if (debug) std::cout << radius << std::endl;


        if (minMaxRadius.first < thisRadius && minMaxRadius.second > thisRadius){
            centerPixel = thisCenterPixel;
            radius = thisRadius;
            return true;
        }



    }

        return false;
}

cv::Mat ximageHandler::getInputImage() const
{
    return inputImage;
}

cv::Mat ximageHandler::getTable()
{
    table = inputImage(ROI);
    return table;
}

void ximageHandler::setMinMaxRadius(float minCM, float maxCM)
{
    minMaxRadius.first = minCM*pixToCm;
    minMaxRadius.second = maxCM*pixToCm;
}

cv::Point2f ximageHandler::getPositionCM() const
{
    cv::Point2f baseToCenter = centerPixel - robotBase;
    return baseToCenter/pixToCm;
}

void ximageHandler::setRobotBase(float xcm, float ycm)
{

    robotBase.x = xcm * pixToCm;
    robotBase.y = ycm * pixToCm;
    if (debug){
        cv::Mat drawing = table;
        cv::circle(drawing, robotBase, 85, cv::Scalar(0, 255, 0), 2 );
        if (debug) cv::imshow("Robot Base", drawing);
    }
}

float ximageHandler::getRadiusCM() const
{
    return radius/pixToCm;
}
