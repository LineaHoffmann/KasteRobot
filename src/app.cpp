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
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        logstd("Thread saying hi!");
    }
}
