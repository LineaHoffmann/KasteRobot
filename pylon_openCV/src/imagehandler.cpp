#include "imagehandler.h"

imageHandler::imageHandler()
{

}

imageHandler::imageHandler(cv::Mat cleanImg)
{
    loadImage(cleanImg);

    if (cutOutTable()){
        std::cout << "succesfull ROI" << std::endl;
    } else {
        std::cout << "failed ROI" << std::endl;
    }
}

void imageHandler::loadImage(cv::Mat image)
{
    image.copyTo(inputImage);
}

bool imageHandler::cutOutTable()
{
        cv::Mat grey;
        cv::cvtColor( inputImage, grey, cv::COLOR_BGR2GRAY ); //Convert to gray
        cv::Mat kernell(5,5, CV_8U, cv::Scalar(1));
        cv::morphologyEx(grey, grey, cv::MORPH_GRADIENT, kernell); //det smart
        if (debug) cv::imshow( "closedimggrey", grey);

        cv::threshold(grey, grey, 35, 255, cv::THRESH_BINARY_INV);

        //double maxValue = 255;
        //double offset = 11;
        //int blockSize = 9;
        //cv::adaptiveThreshold(grey, grey, maxValue, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, blockSize, offset);

        //remove noice
        cv::Mat kernel (2, 2, CV_8U, cv::Scalar(1));
        cv::dilate(grey, grey, kernel);
        cv::erode(grey, grey, kernel);
        cv::medianBlur(grey, grey, 5);
        cv::bitwise_not ( grey, grey );
        if (debug) cv::imshow( "filtering and convert to bin", grey);
        //cv::waitKey();

        // find contours. Makes it easier to sort for what we want.
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(grey,contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

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
        for( int i = 0; i< contours.size(); i++ )
        {
            cv::drawContours(drawing, contours, i, cv::Scalar(255, 255, 255), CV_FILLED);
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
        //cv::waitKey();

        ROI = cv::Rect(topLeftCorner, botRightCorner);

        // Crop the full image to that image contained by the rectangle myROI
        // Note that this doesn't copy the data
        table = inputImage(ROI);
        if (table.cols - inputImage.cols < 0){
            return 1;
        } else {
            return 0;
        }
}

void imageHandler::dectectBall()
{

}

void imageHandler::transformCoordinates()
{

}

cv::Mat imageHandler::getInputImage() const
{
    return inputImage;
}

cv::Mat imageHandler::getTable()
{

    table = inputImage(ROI);
    return table;
}




