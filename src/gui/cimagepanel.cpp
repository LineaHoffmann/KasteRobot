#include "cimagepanel.hpp"

// Local event table
wxBEGIN_EVENT_TABLE(cImagePanel, wxPanel)
    EVT_PAINT(cImagePanel::OnPaintEvent) // When (re)painting the view
    EVT_SIZE(cImagePanel::OnSizeEvent)   // When view is resized
wxEND_EVENT_TABLE()

cImagePanel::cImagePanel(wxWindow *parent,
                         wxStandardID id,
                         wxPoint position,
                         wxSize size)
    : wxPanel(parent, id, position, size, 0, "Camera View") {
    // Load the default image as mCurrentImage, only valid images should overwrite it
    mCurrentImage = wxImage("../resources/defaultImage.png", wxBITMAP_TYPE_ANY);
    mNewImage = wxImage();
    // Defaults for the bools
    mHasNewImage = false;
    mIsDrawing = false;
}
cImagePanel::~cImagePanel() {
    mNewImage.Destroy();
    mCurrentImage.Destroy();
}
void cImagePanel::setNewImage(const wxImage &img) {
    // This is called from a wxTimer, so driven by GUI eventloop
    // Loads image into mNewImage and creates a paint event
    // The reference should not origininate from outside the GUI thread
    if (!img.IsOk()) return;
    if (mNewImage.IsOk()) mNewImage.Clear();
    mNewImage = img.Copy();
    if (!mNewImage.IsOk()) return;
    mHasNewImage = true;
    // Lets post a resize event, then scale will also be re calc'ed
    wxSizeEvent sizeEvent;
    wxPostEvent(this, sizeEvent);
}

void cImagePanel::OnSizeEvent(wxSizeEvent& event) {
    // Resize event handler
    // Keep aspect ratio constant
    // Painting the resized image depends on the wxPaintEvent
    // No actual resizing happens, only calculation of draw scaling
    int32_t viewHeight, viewWidth, imageHeight, imageWidth;
    GetSize(&viewWidth, &viewHeight);
    if (mHasNewImage && mNewImage.IsOk()) {
        imageHeight = mNewImage.GetSize().GetWidth();
        imageWidth = mNewImage.GetSize().GetWidth();
    } else {
        imageHeight = mCurrentImage.GetSize().GetWidth();
        imageWidth = mCurrentImage.GetSize().GetWidth();
    }
    if (!viewHeight || !viewWidth || !imageHeight || !imageWidth) {
        event.Skip();
        return;
    }

    // TODO: [srp] This doesn't handle enough edge cases
    float wScale = (float) viewWidth / (float) imageWidth;
    float hScale = (float) viewHeight / (float) imageHeight;
    if (hScale > wScale) {
        scale = hScale;
    } else {
        scale = wScale;
    }

    Refresh();
    event.Skip();
}
void cImagePanel::OnPaintEvent(wxPaintEvent& event) {
    // Paint event handler
    // Just a matter of creating a DC and calling Draw
    wxPaintDC dc(this);
    Draw(dc);
    event.Skip();
}
void cImagePanel::Draw(wxDC& dc) {
    // Drawing the mCurrentImage
    if (!dc.IsOk()) {return;}
    if (mHasNewImage) {
        // There is a new image in mNewImage
        // Load it into mCurrentImage
        mCurrentImage = mNewImage.Copy();
        mHasNewImage = false;
    }
    if ((mCurrentImage.GetWidth() > 0 || mCurrentImage.GetHeight() > 0) && mCurrentImage.IsOk()) {
        dc.SetUserScale(scale, scale);
        int32_t y = (dc.GetSize().GetHeight() - mCurrentImage.GetHeight()) / 2;
        if (y > 0)
            dc.DrawBitmap(mCurrentImage, 0, y);
        else
            dc.DrawBitmap(mCurrentImage, 0, 0);
    }
}
