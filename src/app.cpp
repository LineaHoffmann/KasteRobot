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
    // cMain is derived from wxFrame
    // Everything is handled in there
    guiMain = new cMain();
    // The privately defined default doesn't play well with my environment
    guiMain->SetSize(1280,1024);
    guiMain->Show();
    logstd("Gui started .. ");
    SetTopWindow(guiMain);
    guiMain->startTimers();
    thread = new std::thread(&app::threadFunc, this);
    return true;
}

void app::threadFunc() {
    //** For testing systemwide threaded updates **//
    //** Dies on ~App **//
    logstd("App thread started!");

    //** Logic init **//
    std::shared_ptr<xBaslerCam> camera = std::make_shared<xBaslerCam>("../resources/pylonimgs/*.bmp", 12500);
    camera->start();
    //** Database init **//

    //** Linking class creation **//
    cLink = std::make_shared<cLinker>();
    xLink = std::make_shared<xLinker>();
    //qLink = std::make_shared<qLinker>();

    //** Class linking **//
    cLink->addLogicLinker(xLink);
    //xLink->addGuiLinker(cLink);
    //xLink->addDatabaseLinker(qLink);
    //qLink->addDatabaseLinker(xLink);

    xLink->addCamera(camera);

    guiMain->addLinker(cLink);
    //logicMain->addLinker(xLink);
    //databaseMain->addLinker(qLink);

    //UR_Control *robot = new xUR_Control("127.0.0.1");
    //xUR_Control *robot = new xUR_Control();
    //xLink->addRobot(robot);


    struct rusage use;
    while (!mJoinThread) {
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // POSIX resource desctription
        if (getrusage(RUSAGE_SELF, &use) == 0) {
            std::string s = "Current App Thread Resource Use [MB]: ";
            s.append(std::to_string(use.ru_maxrss / 1048576.0f));
            logstd(s.c_str());
            if (use.ru_maxrss / 1048576.0f > 10) {
                std::cout << "WARNING: Memory use exceeds 10 MB! This is where my system [srp] starts to chug. Closing program .. " << std::endl;
                this->Exit();
            }
        }

    }
    std::cout << "App thread is dying now .." << std::endl;
}
