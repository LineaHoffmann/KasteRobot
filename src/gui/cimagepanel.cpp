#include "cimagepanel.h"

wxBEGIN_EVENT_TABLE(cImagePanel, wxPanel)
    EVT_PAINT(cImagePanel::OnPaintEvt)
    EVT_SIZE(cImagePanel::OnSize)
wxEND_EVENT_TABLE()
/**
 * @brief Basic constructor
 * @param parent
 * @param id
 */
cImagePanel::cImagePanel(wxWindow* parent, wxStandardID id) : wxPanel(parent, id)
{
    mDefaultImage = new wxImage(wxT("../resources/defaultImage.jpg"), wxBITMAP_TYPE_ANY);
}
cImagePanel::~cImagePanel() {
}
void cImagePanel::AddLinker(cLinker* linker) {
    mLinker = linker;
}
/**
 * @brief wxPaintDC paint-event handler function
 * @param evt
 */
void cImagePanel::OnPaintEvt(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    Draw(dc);
}
/**
 * @brief wxClientDC paint-event handler function
 */
void cImagePanel::OnPaintNow()
{
    wxClientDC dc(this);
    Draw(dc);
}
/**
 * @brief Handler function for resizing events
 * @param evt
 */
void cImagePanel::OnSize(wxSizeEvent &evt) {
    int nWidth, nHeight;
    GetSize(&nWidth, &nHeight);
    int iHeight, iWidth;
    if (mImage == nullptr) {
        iHeight = mDefaultImage->GetSize().GetHeight();
        iWidth = mDefaultImage->GetSize().GetWidth();
    } else {
        iHeight = mImage->GetSize().GetHeight();
        iWidth = mImage->GetSize().GetWidth();
    }
    if (iHeight > 0 && iWidth > 0) {
        mHScale = (float) nWidth / (float) iWidth;
        mWScale = (float) nHeight / (float) iHeight;
        if (mHScale < mWScale) {
            mWScale = mHScale;
        } else {
            mHScale = mWScale;
        }
    }
    Refresh();
}
void cImagePanel::Draw(wxDC &dc) {
    if( !dc.IsOk() || mDrawing == true ){ return; }
    mDrawing = true;
    int x,y,w,h;
    dc.GetClippingBox(&x, &y, &w, &h);
    if( mNewImage )
    {
        *mImage = mLinker->getCameraFrame()->Copy();
        dc.SetUserScale(mHScale, mWScale);
        dc.DrawBitmap(*mImage, x, y);
        mNewImage = false;
    } else if (mImage == nullptr) {
        dc.SetUserScale(mHScale, mWScale);
        dc.DrawBitmap(*mDefaultImage, x, y);
    } else {
        dc.SetUserScale(mHScale, mWScale);
        dc.DrawBitmap(*mImage, x, y);
    }

    mDrawing = false;
    return;
}
void cImagePanel::CheckUpdate()
{
    if (mLinker->hasCameraFrame()) {
        // Set flag for new incoming image, gets loaded in Draw function
        mNewImage = true;
    }
    Update();
}
