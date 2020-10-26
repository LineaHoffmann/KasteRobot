#include "cimagepanel.h"

// Custom event for updating image after extern call to SetNewImage(..)
wxDEFINE_EVENT(CUSTOM_UPDATE_EVENT, wxCommandEvent);

// Local event table
wxBEGIN_EVENT_TABLE(cImagePanel, wxPanel)
    EVT_COMMAND(wxID_ANY, CUSTOM_UPDATE_EVENT, cImagePanel::CheckUpdate)
    EVT_PAINT(cImagePanel::OnPaintEvt)
    EVT_SIZE(cImagePanel::OnSize)
wxEND_EVENT_TABLE()

/**
 * @brief Basic constructor
 * @param parent
 * @param id
 */
cImagePanel::cImagePanel(wxWindow* parent, wxStandardID id) : wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, 0)
{
    mDefaultImage = new wxImage("../resources/defaultImage.jpg", wxBITMAP_TYPE_ANY);
}
cImagePanel::~cImagePanel() {
}
void cImagePanel::addLinker(std::shared_ptr<cLinker> linker) {
    mLinker = linker;
}
void cImagePanel::setNewImage(wxImage img) {
    std::lock_guard<std::mutex> lock(mMtx);
    if (img.IsOk() && !mDrawing) {
        mDrawing = true;
        mNewImage = img.Copy();
        mHasNewImage = true;
        mDrawing = false;
        // This was not run from gui thread, so post an event for it later
        wxCommandEvent evt(CUSTOM_UPDATE_EVENT);
        wxPostEvent(this, evt);
    }
}
/**
 * @brief wxPaintDC paint-event handler function
 * @param evt
 */
void cImagePanel::OnPaintEvt(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    Draw(dc);
    evt.Skip();
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
    evt.Skip();
}
void cImagePanel::Draw(wxDC &dc) {
    if( !dc.IsOk() || mDrawing == true ){ return; }
    mDrawing = true;
    int x,y,w,h;
    dc.GetClippingBox(&x, &y, &w, &h);
    if( mHasNewImage )
    {
        mHasNewImage = false;

        int nWidth, nHeight;
        GetSize(&nWidth, &nHeight);
        int iHeight = mImage->GetSize().GetHeight();
        int iWidth = mImage->GetSize().GetWidth();

        if (iHeight > 0 && iWidth > 0) {
            mHScale = (float) nWidth / (float) iWidth;
            mWScale = (float) nHeight / (float) iHeight;
            if (mHScale < mWScale) {
                mWScale = mHScale;
            } else {
                mHScale = mWScale;
            }
        }

        dc.SetUserScale(mHScale, mWScale);
        dc.DrawBitmap(*mImage, x, y);
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
void cImagePanel::CheckUpdate(wxCommandEvent &evt)
{
    std::lock_guard<std::mutex> lock(mMtx);
    mImage = new wxImage(mNewImage);
    Refresh(false);
    evt.Skip();
}
