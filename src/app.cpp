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

    //** Logic init **//
    std::shared_ptr<xBaslerCam> camera = std::make_shared<xBaslerCam>("../resources/pylonimgs/*.png", 12500);

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

    //** For testing basic inits **//
    //xBaslerCam *camera = new xBaslerCam("../resources/pylonimgs/*.bmp", 12500);
    //xLink->addCamera(camera);
    //UR_Control *robot = new xUR_Control("127.0.0.1");
    //xUR_Control *robot = new xUR_Control();
    //xLink->addRobot(robot);

    thread = new std::thread(&app::threadFunc, this);
    return true;
}
void app::threadFunc() {
    //** For testing systemwide threaded updates **//
    //** Dies on ~App **//
    logstd("App thread started!");
    while (!mJoinThread) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    std::cout << "App thread is dying now .." << std::endl;
}
