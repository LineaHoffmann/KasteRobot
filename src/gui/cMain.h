#ifndef CMAIN_H
#define CMAIN_H

#pragma once
#include "wx/wx.h"
#include "wx/splitter.h"
#include "opencv2/opencv.hpp"
#include "cCamWorker.h"
#include "cImagePanel.h"

class cImagePanel;
class cCamWorker;

class cMain : public wxFrame
{
public:
    cMain();
    ~cMain();

public:
    // Windows for split panels
    // Left side gets all interactive controls
    // Right side gets camera feed and robot orientation
    wxPanel *mLeftPanel = nullptr;
    wxSplitterWindow *mRightSplitpanel = nullptr;

    // GUI elements
    wxButton *mBtn1 = nullptr;
    wxButton *mBtn2 = nullptr;
    wxTextCtrl *mText1 = nullptr;
    wxListBox *mList1 = nullptr;

    // Top menu bar
    wxMenuBar *mMenuBar = nullptr;

    // Viewing area for camera feed
    cImagePanel *mCameraPanel = nullptr;
    // Pointer to camera
    //cCamera* mCamera = nullptr;
    // Pointer to camera worker thread
    cCamWorker* mCamWorker = nullptr;

    // Viewing area for robot orientation
    cImagePanel *mRobotPanel = nullptr;
    // Pointer to robot object
    //cRobot* mRobot = nullptr;
    // Pointer to robot worker thread
    //cRobotWorker mRobotWorker = nullptr;

    // Base functionality funtions
    void OnBtn1Clicked(wxCommandEvent &evt);
    void OnBtn2Clicked(wxCommandEvent &evt);

    void OnMenuSaveLog(wxCommandEvent &evt);
    void OnMenuSaveSnap(wxCommandEvent &evt);
    void OnMenuExit(wxCommandEvent &evt);
    void OnMenuAbout(wxCommandEvent &evt);

    // Just a macro to "enable" events
    wxDECLARE_EVENT_TABLE();
};

#endif // CMAIN_H
