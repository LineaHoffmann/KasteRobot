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
    //** GUI init **//
    // Just has to be called
    wxInitAllImageHandlers();

    // GUI start
    guiMain = new cMain();
    guiMain->Show();
    logstd("Gui started .. ");
    SetTopWindow(guiMain);

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

    struct rusage use;
    while (!mJoinThread) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // POSIX resource desctription
        if (getrusage(RUSAGE_SELF, &use) == 0) {
            std::string s = "Current App Thread Resource Use [MB]: ";
            s.append(std::to_string(use.ru_maxrss / 1048576.0f));
            guiMain->pushStrToStatus(s);
            //std::cout << s << std::endl;
            if (use.ru_maxrss / 1048576.0f > 10) {
                std::cout << "WARNING: Memory use exceeds 10 MB! This is where my system [srp] starts to chug. Closing program .. " << std::endl;
                this->Exit();
            }
        }
    }
    std::cout << "App thread is dying now .." << std::endl;
}
