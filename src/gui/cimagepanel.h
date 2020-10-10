#ifndef CIMAGEPANEL_H
#define CIMAGEPANEL_H
#pragma once

#include "../includeheader.h"
#include "cimagepanel.h"
#include "clinker.h"

class cImagePanel : public wxPanel
{
public:
    cImagePanel(wxWindow* parent, wxStandardID id);
    ~cImagePanel();
    void addLinker(cLinker* linker);
    void setNewImage(wxImage img);

private:
    // These functions get to keep uppercase first letter ..
    void OnPaintNow();
    void OnPaintEvt(wxPaintEvent &evt);
    void OnSize(wxSizeEvent& evt);
    void Draw(wxDC& dc);
    void CheckUpdate(wxCommandEvent &evt);

    bool mDrawing = false;

    float mHScale{0}, mWScale{0};

    wxImage mNewImage;
    bool mHasNewImage = false;
    wxImage *mImage = nullptr;
    wxImage *mDefaultImage = nullptr;
    cLinker *mLinker = nullptr;

    std::mutex mMtx;

    wxDECLARE_EVENT_TABLE();
};

#endif // CIMAGEPANEL_H
