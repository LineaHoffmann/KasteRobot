#include "app.h"

wxIMPLEMENT_APP(app);

// This class is the application itself
// To edit the GUI go to cmain.cpp

app::app()
{
}
app::~app()
{
    // Generally nothing GUI related should be manually destroyed
    // The wx package handles destruction of any elements it relies on
    // Everything else must go
    mJoinThread = true;

    thread->join();
    delete thread;
}
bool app::OnInit() {
    // Just has to be called
    wxInitAllImageHandlers();
    // cMain is derived from wxFrame
    // Everything is handled in there
    mFrame = new cMain();
    // The privately defined default doesn't play well with my environment
    mFrame->SetSize(1280,1024);
    mFrame->Show();
    logstd("Gui started .. ");

    thread = new std::thread(&app::threadFunc, this);

    return true;
}
void app::threadFunc() {
    logstd("Thread trying out the camera!");

//    xBaslerCam camera("../resources/pylonimgs/*.bmp", 12500);
//    if (camera.start()) {
//        logstd("Camera started succesfully ..");
//    } else {
//        logerr("Camera failed to start! A test image will be displayed instead ..");
//    }

//    //cv::imshow("Test Image", camera.getImage());

//    if (camera.isConnected()) {
//        logstd("Camera connected succesfully ..");
//    } else {
//        logerr("Camera is not connecteed ..");
//    }
    while (!mJoinThread) {
        logstd("Thread is waiting!");
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    std::cout << "Thread is dying now .." << std::endl;
}
