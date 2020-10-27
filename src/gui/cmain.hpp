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
#include <sys/resource.h> // For resource use tracking


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
#include <wx/gbsizer.h>

#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

#include "cimagepanel.hpp"

#include "idbindings.h"
#include "../logic/xcontroller.hpp"

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

    // Template function for wrapping calls in most basic x_err / std::exception handling
    // Credit must go to StackExchange users Morwenn and Tim Martin
    // https://codereview.stackexchange.com/questions/2484/generic-c-exception-catch-handler-macro
    // Takes a lambda as argument and optionally an appended message
    // Minimum C++11 for decltype()
    template<typename Callable>
    auto xTry(Callable&& func, const std::string& msg="") -> decltype(func()) {
        try {
            return func();
        } catch (const x_err::error &e) {
            logerr((std::string(e.what()).append(msg)).c_str());
        } catch (const std::exception &e) {
            logerr((std::string(e.what()).append(msg)).c_str());
        }
    }

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
    wxTreeListItem *mTreeRobotPosition;
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
    // ROBOT SUB PANEL ENTRIES
    wxButton *mBtnRobotConnect;
    wxButton *mBtnRobotDisconnect;
    wxButton *mBtnRobotUpdate;
    wxButton *mBtnRobotSendCmd;
    wxButton *mBtnRobotSendPos;
    wxTextCtrl *mTxtRobotIP;
    wxTextCtrl *mTxtRobotPort;
    wxTextCtrl *mTxtRobotCmd;
    wxTextCtrl *mTxtRobotX;
    wxTextCtrl *mTxtRobotY;
    wxTextCtrl *mTxtRobotZ;
    wxTextCtrl *mTxtRobotRX;
    wxTextCtrl *mTxtRobotRY;
    wxTextCtrl *mTxtRobotRZ;
    // CAMERA SUB PANEL ENTRIES
    wxButton *mBtnCameraConnect;
    wxButton *mBtnCameraDisconnect;
    wxTextCtrl *mTxtCameraExposure;
    // GRIPPER SUB PANEL ENTRIES
    wxButton *mBtnGripperConnect;
    wxButton *mBtnGripperDisconnect;
    wxTextCtrl *mTxtGripperIP;
    wxTextCtrl *mTxtGripperPort;
    // DATABASE SUB PANEL ENTRIES
    wxButton *mBtnDatabaseConnect;
    wxButton *mBtnDatabaseDisconnect;
    wxTextCtrl *mTxtDatabaseIP;
    wxTextCtrl *mTxtDatabasePort;
    wxTextCtrl *mTxtDatabaseUser;
    wxTextCtrl *mTxtDatabasePassword;
    wxTextCtrl *mTxtDatabaseSchema;

    // Just a macro to enable wxWidgets events
    wxDECLARE_EVENT_TABLE();
};

#endif // CMAIN_H
