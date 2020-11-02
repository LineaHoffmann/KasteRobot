//#define __GXX_ABI_VERSION 1011
#include "app.hpp"

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

    mJoinThread.exchange(true);
    thread->join();
    delete thread;
}
bool app::OnInit() {
    //** GUI init **//
    // Just has to be called
    wxInitAllImageHandlers();

    // GUI start
    guiMain = std::make_unique<cMain>();
    guiMain->Show();
    logstd("Gui started .. ");
    SetTopWindow(guiMain.get());

    mJoinThread.exchange(false);
    thread = new std::thread(&app::threadFunc, this);

    guiMain->PushStatusText("Running .. ");
    return true;
}

void app::threadFunc() {
    //** For testing systemwide threaded updates **//
    //** Dies on ~App **//
    logstd("App thread started!");

    std::shared_ptr<xController> controller = std::make_shared<xController>();
    guiMain->setLogicControllerPointer(controller);


    while (!mJoinThread.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

    }
    std::cout << "App thread is dying now .." << std::endl;
}
