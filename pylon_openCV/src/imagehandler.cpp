#include "imagehandler.h"

imageHandler::imageHandler()
{

}

void imageHandler::loadImage(cv::Mat image)
{
    image.copyTo(inputImage);
}

bool imageHandler::cutOutTable()
{
    {
        int largest_area=0;
        int largest_contour_index=0;
        cv::Rect bounding_rect;
        //63, 66, 83,115

        cv::Mat grey;
        cv::cvtColor( inputImage, grey, cv::COLOR_BGR2GRAY ); //Convert to gray
        cv::Mat kernell(5,5, CV_8U, cv::Scalar(1));
        cv::morphologyEx(grey, grey, cv::MORPH_GRADIENT, kernell);

        cv::imshow( "closedimggrey", grey);

//        //test
//        std::vector<cv::KeyPoint> keypointsD;
//        cv::Ptr<cv::FastFeatureDetector> detector=cv::FastFeatureDetector::create();
//        std::vector<cv::Mat> descriptor;

//        detector->detect(drawing,keypointsD,cv::Mat());
//        cv::drawKeypoints(drawing, keypointsD, drawing);
//        cv::imshow("keypoints",drawing);
//        cv::waitKey();




//        //test2
//        cv::Mat egdes;
//        cv::Canny(grey, egdes, 30, 150);

//        cv::imshow("canny", egdes);
//        grey = egdes;
//        double maxValue = 255;
//        double offset = 35;
//        int blockSize = 5;

        cv::threshold(grey, grey, 35, 255, cv::THRESH_BINARY_INV);
        //cv::threshold(grey, grey, 50, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
        //cv::adaptiveThreshold(grey, grey, maxValue, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, blockSize, offset);

        cv::Mat kernel (2, 2, CV_8U, cv::Scalar(1));

        //cv::dilate(thresholdkernel, thresholdkernel, kernel);
        //cv::erode(grey, grey, kernel);
        //cv::medianBlur(grey, grey, 5);
        cv::bitwise_not ( grey, grey );


        //test3
        cv::Mat position = grey;
        // Setup a rectangle to define your region of interest
        cv::Rect myROI(10, 10, 100, 100);

        std::vector<cv::KeyPoint> whereAmI;

        for (int j  = grey.rows; j > 0 ; j-=150){
            for (int i  = 0; i < grey.cols; i+=150){

                cv::KeyPoint test(i, j, 5);
                whereAmI.push_back(test);

                cv::drawKeypoints(position, whereAmI, position);
                cv::imshow("test3", position );
                cv::waitKey();

            }
        }








        // Crop the full image to that image contained by the rectangle myROI
        // Note that this doesn't copy the data
        //cv::Mat croppedImage = image(myROI);


        cv::imshow( "result1", grey);
        cv::waitKey();

        cv::Mat mask = grey;

        // find contours (if always so easy to segment as your image, you could just add the black/rect pixels to a vector)
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(mask,contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        //remove small contours
        for (std::vector<std::vector<cv::Point> >::iterator it = contours.begin(); it!=contours.end(); )
        {
            if (it->size()<500)
                it=contours.erase(it);
            else
                ++it;
        }




        // Draw contours and find biggest contour (if there are other contours in the image, we assume the biggest one is the desired rect)
        // drawing here is only for demonstration!
        int biggestContourIdx = -1;
        float biggestContourArea = 0;
        cv::Mat drawing = cv::Mat::zeros( mask.size(), CV_8UC3 );
        for( int i = 0; i< contours.size(); i++ )
        {
            cv::Scalar color = cv::Scalar(255, 50, 50);
            cv::drawContours( drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point() );

            float ctArea= cv::contourArea(contours[i]);
            if(ctArea > biggestContourArea)
            {
                biggestContourArea = ctArea;
                biggestContourIdx = i;
            }
        }


        // if no contour found
        if(biggestContourIdx < 0)
        {
            std::cout << "no contour found" << std::endl;
            return 1;
        }

        // compute the rotated bounding rect of the biggest contour! (this is the part that does what you want/need)
        //cv::RotatedRect boundingBox = cv::minAreaRect(contours[biggestContourIdx]);
        // one thing to remark: this will compute the OUTER boundary box, so maybe you have to erode/dilate if you want something between the ragged lines
        // Find the minimum area enclosing bounding box
        cv::Point2f vtx[4];
        cv::RotatedRect box = cv::minAreaRect(contours[biggestContourIdx]);
        box.points(vtx);

        // Draw the bounding box
        for(int i = 0; i < 4; i++ )
            cv::line(drawing, vtx[i], vtx[(i+1)%4], cv::Scalar(0, 255, 0), 1, cv::LINE_AA);



        // display
        cv::imshow("drawing", drawing);
        cv::waitKey(0);

    }
}

cv::Mat imageHandler::getInputImage() const
{
    return inputImage;
}
