#ifndef CIMAGEPANEL_H
#define CIMAGEPANEL_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include "wx/splitter.h"

class cImagePanel : public wxPanel
{
    wxImage image;
    wxBitmap resized;
    int w,h;

public:
    cImagePanel(wxWindow* parent, wxStandardID id, wxString file, wxBitmapType format);

    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void OnSize(wxSizeEvent& evt);
    void render(wxDC& dc);

    wxDECLARE_EVENT_TABLE();
};

#endif // CIMAGEPANEL_H
