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
    delete xLink;
    delete cLink;
}
bool app::OnInit() {

    cLink = new cLinker;
    xLink = new xLinker;

    cLink->addLogicLinker(xLink);

    xBaslerCam *camera = new xBaslerCam("../resources/pylonimgs/*.bmp", 12500);
    xLink->addCamera(camera);

    // Just has to be called
    wxInitAllImageHandlers();
    // cMain is derived from wxFrame
    // Everything is handled in there
    mFrame = new cMain();
    mFrame->addLinker(cLink);
    // The privately defined default doesn't play well with my environment
    mFrame->SetSize(1280,1024);
    mFrame->Show();
    logstd("Gui started .. ");

    thread = new std::thread(&app::threadFunc, this);

    return true;
}
void app::threadFunc() {
    logstd("Thread started!");

    while (!mJoinThread) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    std::cout << "Thread is dying now .." << std::endl;
}
