#include "ximagehandler.h"

xImageHandler::xImageHandler()
{

}

xImageHandler::xImageHandler(cv::Mat cleanImg)
{
    loadImage(cleanImg);

    if (cutOutTable()){
        std::cout << "succesfull ROI" << std::endl;
    } else {
        std::cout << "failed ROI" << std::endl;
    }
}

void xImageHandler::loadImage(cv::Mat image)
{
    image.copyTo(inputImage);
}

bool xImageHandler::cutOutTable()
{

    cv::Mat grey;
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
    for( uint i = 0; i< contours.size(); i++ )
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
        return 1;
    } else {
        return 0;
    }
}

void xImageHandler::dectectBall() {
    cv::Mat hsv_image;
    cv::cvtColor(table, hsv_image, cv::COLOR_BGR2HLS);

    // Threshold the HSV image, keep only the red pixels
    cv::Mat lower_red_hue_range;
    cv::Mat upper_red_hue_range;
    cv::Mat bin;
    cv::inRange(hsv_image, cv::Scalar(0, 100, 120), cv::Scalar(30, 240, 255), lower_red_hue_range);
    cv::inRange(hsv_image, cv::Scalar(170, 100, 120), cv::Scalar(179, 240, 255), upper_red_hue_range);
    cv::addWeighted(lower_red_hue_range, 1, upper_red_hue_range, 1, 0, bin);

    //remove noice
    cv::Mat kernel (3, 3, CV_8U, cv::Scalar(1));

    cv::erode(bin, bin, kernel);
    cv::dilate(bin, bin, kernel);
    cv::medianBlur(bin, bin, 7);

    //find contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point2f> centers;
    std::vector<float> radius;

    cv::Point2f ctest;
    float rtest;
    cv::findContours(bin,contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //remove small contours
    for (std::vector<std::vector<cv::Point> >::iterator it = contours.begin(); it!=contours.end(); )
    {
        if (it->size()<20) {
            it=contours.erase(it);
        } else {
            ++it;
        }
    }

    //draw filled contours of whats left.
    cv::Mat drawing = cv::Mat::zeros( bin.size(), CV_8UC3 );
    for( uint i = 0; i< contours.size(); i++ )
    {
        cv::minEnclosingCircle(contours[i], ctest, rtest);
        cv::circle(drawing, ctest, rtest, cv::Scalar(0, 255, 0), 2 );
    }


    cv::imshow("filted", drawing);
}

void xImageHandler::transformCoordinates()
{

}

cv::Mat xImageHandler::getInputImage() const
{
    return inputImage;
}

cv::Mat xImageHandler::getTable()
{

    table = inputImage(ROI);
    return table;
}
