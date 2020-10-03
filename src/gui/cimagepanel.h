#ifndef CIMAGEPANEL_H
#define CIMAGEPANEL_H

#include <thread>
#include <mutex>

#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/splitter.h"
#include "clinker.h"

class cImagePanel : public wxPanel
{
public:
    cImagePanel(wxWindow* parent, wxStandardID id);
    ~cImagePanel();
    void AddLinker(cLinker* linker);
    void CheckUpdate();

    bool mNewImage = false;
    wxImage *mImage = nullptr;

private:
    void OnPaintNow();
    void OnPaintEvt(wxPaintEvent &evt);
    void OnSize(wxSizeEvent& evt);
    void Draw(wxDC& dc);

    bool mDrawing = false;

    float mHScale{0}, mWScale{0};

    wxImage *mDefaultImage = nullptr;
    cLinker *mLinker = nullptr;

    wxDECLARE_EVENT_TABLE();
};

#endif // CIMAGEPANEL_H
