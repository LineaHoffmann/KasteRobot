#pragma once
#ifndef CIMAGEPANEL_H
#define CIMAGEPANEL_H

#include <atomic>

#include "wx/wx.h"

class cImagePanel : public wxPanel
{
    wxDECLARE_EVENT_TABLE();
public:
    cImagePanel(wxWindow *parent,
                wxStandardID id = wxID_ANY,
                const wxPoint &position = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                const wxString &name = "");
    ~cImagePanel();

    /**
     * @brief Copy image into viewing panel
     * @param Const reference to local temporary wxImage
     */
    void setNewImage(const wxImage& img);

private:
    void OnSizeEvent(wxSizeEvent& event); // Resizing event handler
    void OnPaintEvent(wxPaintEvent& event); // Paint event handler
    void Draw(wxDC& dc);    // Draws mCurrentImage to the view

    float scale = 1.0f;
    std::atomic<bool> mHasNewImage; // For checking if a new image has been copied to mNewImage
    std::atomic<bool> mIsDrawing;   // For checking if we are currently drawing an image
    wxImage mCurrentImage;          // Currently displayed image
    wxImage mNewImage;              // New image loaded from setNewImage(..)
};

#endif // CIMAGEPANEL_H
