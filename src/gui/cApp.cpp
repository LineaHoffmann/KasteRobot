#include "cApp.h"

wxIMPLEMENT_APP(cApp);

// This class is the application itself, not the actual UI
// To edit the UI go to cMain.cpp

cApp::cApp()
{
}
cApp::~cApp()
{
}

bool cApp::OnInit() {

    // Just has to be called
    wxInitAllImageHandlers();

    // cMain is derived from wxFrame
    // Everything is handled in there
    mFrame = new cMain();
    mFrame->Show();

    return true;
}
