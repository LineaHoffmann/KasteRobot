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
    inputImage = image.clone();
}

bool ximageHandler::cutOutTable()
{
    cv::Mat grey;
    //if (debug) cv::imshow( "input", inputImage);
    cv::cvtColor( inputImage, grey, cv::COLOR_BGR2GRAY ); //Convert to gray
    cv::Mat kernell(5,5, CV_8U, cv::Scalar(1));
    cv::morphologyEx(grey, grey, cv::MORPH_GRADIENT, kernell); //det smart
    //if (debug)
    //cv::imshow( "closedimggrey", grey);

    cv::threshold(grey, grey, 25, 255, cv::THRESH_BINARY_INV);

    //remove noice
    cv::Mat kernel (2, 2, CV_8U, cv::Scalar(1));
    cv::dilate(grey, grey, kernel);
    cv::erode(grey, grey, kernel);
    cv::medianBlur(grey, grey, 5);
    cv::bitwise_not ( grey, grey );
    //if (debug) cv::imshow( "filtering and convert to bin", grey);

    // find contours. Makes it easier to sort for what we want.
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(grey,contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //remove small contours, mostly all the cirles on the table.
    for (std::vector<std::vector<cv::Point> >::iterator it = contours.begin(); it!=contours.end(); )
    {
        if (it->size()<120)
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
    int delta = 3;
    int offset = 400;
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
        if(thisCount > offset && thisCount - lastCount < delta) {
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
        if(thisCount > offset && thisCount - lastCount < delta) {
            break;
        }
        lastCount = thisCount;
    }

    cv::line(drawing, topLeftCorner, botLeftCorner, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
    cv::line(drawing, topRightCorner, botRightCorner, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);

    //if (debug)
    //cv::imshow("where it found table egdes", drawing);




    // Crop the full image to that image contained by the rectangle myROI
    // Note that this doesn't copy the data
    ROI = cv::Rect(topLeftCorner, botRightCorner);
    table = inputImage(ROI);
    if (table.cols - inputImage.cols < 0){
        //pixel to cm hackerway
        pixToCm = table.cols / tableWidth;

        if (showResult)cv::imshow("table", table);
        return 1;
    } else {
        return 0;
    }
}

std::tuple<bool, cv::Mat, cv::Point2f, float> ximageHandler::findBallAndPosition(cv::Mat image)
{
    loadImage(image);
    std::pair<bool, cv::Mat> result = detectBall();
    if (result.first){

    }else {
        logstd("no ball found");
    }
    std::tuple<bool, cv::Mat, cv::Point2f, float> data;
    std::get<0>(data) = result.first;
    std::get<1>(data) = result.second;
    std::get<2>(data) = getPositionCM();
    std::get<3>(data) = getRadiusCM();

    if (result.first){
        std::stringstream s;
        s.str(std::string()); // Reset the stringstream
        s << "ball position: " << std::get<2>(data) << " || radius: " << std::get<3>(data);
        logstd(s.str().c_str());

    }else {
        logstd("no ball found");
    }


    return data;
}


void ximageHandler::ballColor(int hue, int spread)
{
    //std::vector<int> colorRange = {0,30,170,180};
    colorRange.clear();
    if (hue-spread < 0){
        colorRange.push_back(0);
        colorRange.push_back(hue+spread);
        colorRange.push_back(180-spread+hue);
        colorRange.push_back(180);
        return;

    } else {
        colorRange.push_back(hue-spread);
        colorRange.push_back(hue);
        colorRange.push_back(hue+1);
        colorRange.push_back(hue+spread);
    }
}

std::pair<bool, cv::Mat> ximageHandler::detectBall()
{
    cv::Mat hsv_image;
    cv::cvtColor(inputImage(ROI), hsv_image, cv::COLOR_BGR2HSV);


    // Threshold the HSV image, keep only the red pixels
    cv::Mat lower_red_hue_range;
    cv::Mat upper_red_hue_range;
    cv::Mat bin;
    cv::inRange(hsv_image, cv::Scalar(colorRange[0], 80, 80), cv::Scalar(colorRange[1], 255, 255), lower_red_hue_range);
    cv::inRange(hsv_image, cv::Scalar(colorRange[2], 80, 80), cv::Scalar(colorRange[3], 255, 255), upper_red_hue_range);
    cv::addWeighted(lower_red_hue_range, 1, upper_red_hue_range, 1, 0, bin);

    //remove noice
    cv::Mat kernel (3, 3, CV_8U, cv::Scalar(1));
    cv::erode(bin, bin, kernel);
    cv::erode(bin, bin, kernel);
    cv::dilate(bin, bin, kernel);
    cv::medianBlur(bin, bin, 7);

    //find contours
    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(bin,contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //remove small contours
    for (std::vector<std::vector<cv::Point> >::iterator it = contours.begin(); it!=contours.end(); )
    {
        if (it->size()<(15)) {
            it=contours.erase(it);
        } else {
            ++it;
        }
    }

    cv::Point2f thisCenterPixel;
    float thisRadius;
    //draw filled contours of whats left.
    cv::Mat drawing = inputImage;
    if (showResult) cv::circle(drawing(ROI), robotBase, 85, cv::Scalar(255, 255, 255), 2 );


    for(unsigned int i = 0; i< contours.size(); i++ )
    {
        cv::minEnclosingCircle(contours[i], thisCenterPixel, thisRadius);

        if ((minMaxRadius.first < thisRadius) && (minMaxRadius.second > thisRadius)){
            centerPixel = thisCenterPixel;
            radius = thisRadius;

            if (showResult) cv::circle(drawing(ROI), thisCenterPixel, thisRadius, cv::Scalar(0, 255, 0), 2 );

            return std::pair<bool, cv::Mat>(true, drawing);
        }
    }
    return std::pair<bool, cv::Mat>(false, drawing);
}

cv::Mat ximageHandler::getInputImage() const
{
    return inputImage;
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
}

float ximageHandler::getRadiusCM() const
{
    return radius/pixToCm;
}
