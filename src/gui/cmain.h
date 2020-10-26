#ifndef CMAIN_H
#define CMAIN_H
#pragma once

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include <string>
#include <vector>
#include <chrono>
#include <utility>
#include <thread>
#include <mutex>

#include <ctime> // For timestamp, C++20 isn't showing up :((

#include "wx/wx.h"
#include "wx/app.h"
#include "wx/frame.h"
#include "wx/menu.h"
#include "wx/sizer.h"
#include "wx/splitter.h"
#include "wx/textctrl.h"
#include "wx/notebook.h"
#include "wx/treelist.h"
#include "wx/statusbr.h"
#include "wx/aboutdlg.h"
#include "wx/artprov.h"

#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

#include "cimagepanel.h"

#include "../logic/xcontroller.h"

enum FUNCTION_BINDING_ID {
    ID_TIMER_VIEW1_UPDATE,      // Updating for view 1
    ID_TIMER_VIEW2_UPDATE,      // Not used at the moment
    ID_TIMER_INFOTREE_UPDATE,   // Updating the information tree

    ID_MENU_SAVE_LOG,           // Menu buttons
    ID_MENU_SAVE_SNAPSHOT,
    ID_MENU_EXIT,
    ID_MENU_ABOUT,

    ID_BTN_ROBOT_CONNECT,       // Notebook panel buttons
    ID_BTN_ROBOT_DISCONNECT,
    ID_BTN_GRIPPER_CONNECT,
    ID_BTN_GRIPPER_DISCONNECT,
    ID_BTN_CAMERA_CONNECT,
    ID_BTN_CAMERA_DISCONNECT,
    ID_BTN_DATABASE_CONNECT,
    ID_BTN_DATABASE_DISCONNECT
};

class cMain : public wxFrame
{
public:
    cMain();
    ~cMain();


    void pushStrToStatus(std::string& msg); // Thread safe
    void popStrFromStatus(); // Thread safe
    void setLogicControllerPointer(std::shared_ptr<xController> controller); // Thread safe

private:
    // GUI event handler functions, linked in top of cMain.cpp
    // They get to keep uppercase first letter
    void OnTimerView1Update(wxTimerEvent &evt);
    void OnTimerView2Update(wxTimerEvent &evt);
    void OnTimerInfoUpdate(wxTimerEvent &evt);
    void OnButtonPress(wxCommandEvent &evt);

private:
    // Pointer to logic controller object
    std::shared_ptr<xController> mController;

    // Main window
    cImagePanel *mPanelView1;  // Panel for view 1
    cImagePanel *mPanelView2;  // Panel for view 2

    // Logging area
    wxTextCtrl *mTextCtrl;
    wxLogTextCtrl *mTextLog;

    // PanelNotebook
    wxNotebook *mNotebook;
    wxTreeListCtrl *mTreeList;

    // About box
    wxAboutDialogInfo *mAboutBox;

    // Top menu bar
    wxMenuBar *mMenuBar;

    // Bottom status bar
    wxStatusBar *mStatusBar;

    // Update timers
    wxTimer mTimerView1;
    //wxTimer mTimerView2;
    wxTimer mTimerInfo;

    std::mutex mMtx;

    // Data entries for the tree view
    wxTreeListItem *mTreeRootRobot;
    wxTreeListItem *mTreeRootCamera;
    wxTreeListItem *mTreeRootGripper;
    wxTreeListItem *mTreeRootDatabase;
    wxTreeListItem *mTreeRobotState;
    wxTreeListItem *mTreeRobotIP;
    wxTreeListItem *mTreeRobotPort;
    wxTreeListItem *mTreeCameraState;
    wxTreeListItem *mTreeGripperState;
    wxTreeListItem *mTreeGripperIP;
    wxTreeListItem *mTreeGripperPort;
    wxTreeListItem *mTreeDatabaseState;
    wxTreeListItem *mTreeDatabaseIP;
    wxTreeListItem *mTreeDatabasePort;
    wxTreeListItem *mTreeDatabaseName;
    wxTreeListItem *mTreeDatabaseSchema;

    // Objects for the notebook tabs (buttons, fields, etc)
    wxButton *mBtnRobotConnect;
    wxButton *mBtnRobotDisconnect;
    wxButton *mBtnCameraConnect;
    wxButton *mBtnCameraDisconnect;
    wxButton *mBtnGripperConnect;
    wxButton *mBtnGripperDisconnect;
    wxButton *mBtnDatabaseConnect;
    wxButton *mBtnDatabaseDisconnect;
    wxTextCtrl *mTxtRobotIP;
    wxTextCtrl *mTxtRobotPort;
    wxTextCtrl *mTxtCameraExposure;
    wxTextCtrl *mTxtGripperIP;
    wxTextCtrl *mTxtGripperPort;
    wxTextCtrl *mTxtDatabaseIP;
    wxTextCtrl *mTxtDatabasePort;
    wxTextCtrl *mTxtDatabaseUser;
    wxTextCtrl *mTxtDatabasePassword;
    wxTextCtrl *mTxtDatabaseSchema;

    // Just a macro to enable wxWidgets events
    wxDECLARE_EVENT_TABLE();
};

#endif // CMAIN_H
