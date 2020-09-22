#ifndef CMAIN_H
#define CMAIN_H

#pragma once
#include "wx/wx.h"
#include "wx/splitter.h"
#include "wx/notebook.h"
#include "opencv2/opencv.hpp"
#include "cCamWorker.h"
#include "cImagePanel.h"

class cMain : public wxFrame
{
public:
    cMain();
    ~cMain();

    // GUI event handler functions, linked in top of cMain.cpp
    void OnBtn1Clicked(wxCommandEvent &evt);
    void OnBtn2Clicked(wxCommandEvent &evt);
    void OnMenuSaveLog(wxCommandEvent &evt);
    void OnMenuSaveSnap(wxCommandEvent &evt);
    void OnMenuExit(wxCommandEvent &evt);
    void OnMenuAbout(wxCommandEvent &evt);

private:
    // Functions for keeping the main constructor readable
    void initSizers();
    void initMainWindow();
    void initMenu();

    // Functions for keeping the fuctions for keeping the main constructor readable, readable ...
    void initLeftPanel();
    void initRightPanel();

    // Functions for keeping the fuctions for the fuctions for keeping the main... :)
    void initTabGeneral();
    void initTabRobot();
    void initTabGripper();
    void initTabCamera();
    void initTabDatabase();

public:
    // Windows for split panels
    wxSplitterWindow *mSplitterMain = nullptr;

    // Left side gets all interactive controls
    wxNotebook *mLeftBookPanel = nullptr;
    wxPanel *mLeftSubPanelGeneral = nullptr;
    wxPanel *mLeftSubPanelRobot = nullptr;
    wxPanel *mLeftSubPanelGripper = nullptr;
    wxPanel *mLeftSubPanelCamera = nullptr;
    wxPanel *mLeftSubPanelDatabase = nullptr;

    // Right side gets camera feed and robot orientation
    wxSplitterWindow *mRightSplitpanel = nullptr;

    // GUI sizers
    wxBoxSizer *mSizerMain = nullptr;
    wxBoxSizer *mSizerRight = nullptr;
    wxBoxSizer *mSizerLeft = nullptr;
    wxBoxSizer *mSizerLeftGeneral = nullptr;
    wxBoxSizer *mSizerLeftRobot = nullptr;
    wxBoxSizer *mSizerLeftGripper = nullptr;
    wxBoxSizer *mSizerLeftCamera = nullptr;
    wxBoxSizer *mSizerLeftDatabase = nullptr;

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
    //cCamWorker* mCamWorker = nullptr;

    // Viewing area for robot orientation
    cImagePanel *mRobotPanel = nullptr;
    // Pointer to robot object
    //cRobot* mRobot = nullptr;
    // Pointer to robot worker thread
    //cRobotWorker mRobotWorker = nullptr;

    // Just a macro to enable wxWidgets events
    wxDECLARE_EVENT_TABLE();
};

#endif // CMAIN_H
