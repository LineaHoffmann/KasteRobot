#include "capp.h"

// We're not getting cout and cerr to GUI, wxLog does it much easier
// Shortening the names a bit for ease of logging, so log as 'logXXX("This shows in GUI");'
#ifndef logstd
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

wxIMPLEMENT_APP(cApp);

// This class is the application itself
// To edit the GUI go to cMain.cpp

cApp::cApp()
{
}
cApp::~cApp()
{
    // Generally nothing GUI related should be manually destroyed
    // The wx package handles destruction of any elements it relies on
}
bool cApp::OnInit() {
    // Just has to be called
    wxInitAllImageHandlers();
    // cMain is derived from wxFrame
    // Everything is handled in there
    mFrame = new cMain();
    // The privately defined default doesn't play well with my gtk2 environment
    mFrame->SetSize(1280,1024);
    mFrame->Show();
    logstd("Gui started .. ");
    logwar("Gui started .. ");
    logerr("Gui started .. ");

    thread = new std::thread(&cApp::threadFunc, this);

    return true;
}
void cApp::threadFunc() {
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        logstd("Thread saying hi!");
    }
}
