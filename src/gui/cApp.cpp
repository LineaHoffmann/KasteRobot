#include "cApp.h"

wxIMPLEMENT_APP(cApp);

// This class is the application itself, not the actual UI
// To edit the UI go to cMain.cpp

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

    return true;
}
