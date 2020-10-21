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
#include <utility>
#include <thread>
#include <mutex>

#include "wx/wx.h"
#include "wx/app.h"
#include "wx/frame.h"
#include "wx/menu.h"
#include "wx/sizer.h"
#include "wx/splitter.h"
#include "wx/textctrl.h"
#include "wx/notebook.h"
#include "wx/treelist.h"
#include "wx/aboutdlg.h"
#include "wx/artprov.h"

#include "clinker.h"
#include "cimagepanel.h"

#include "../logic/xrobotexceptions.h"

enum FUNCTION_BINDING_IDS {
    ID_TIMER_CAMERA_UPDATE,

    ID_MENU_SAVE_LOG,
    ID_MENU_SAVE_SNAPSHOT,
    ID_MENU_EXIT,
    ID_MENU_ABOUT,

    ID_BTN_ROBOT_CONNECT,
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

    void addLinker(std::shared_ptr<cLinker> linker);

private:
    // GUI event handler functions, linked in top of cMain.cpp
    // They get to keep uppercase first letter
    void OnTimerCameraUpdate(wxTimerEvent &evt);
    // Main window top bar functions
    void OnMenuSaveLog(wxCommandEvent &evt);
    void OnMenuSaveSnap(wxCommandEvent &evt);
    void OnMenuExit(wxCommandEvent &evt);
    void OnMenuAbout(wxCommandEvent &evt);
    // Left panel controls handler functions
    void OnBtnRobotConnect(wxCommandEvent &evt);
    void OnBtnRobotDisconnect(wxCommandEvent &evt);
    void OnBtnGripperConnect(wxCommandEvent &evt);
    void OnBtnGripperDisconnect(wxCommandEvent &evt);
    void OnBtnCameraConnect(wxCommandEvent &evt);
    void OnBtnCameraDisconnect(wxCommandEvent &evt);
    void OnBtnDatabaseConnect(wxCommandEvent &evt);
    void OnBtnDatabaseDisconnect(wxCommandEvent &evt);

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

private:
    // Main window
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

    // GUI elements for general tab
    wxTreeListCtrl *mTabGeneralTreeList = nullptr;

    wxTreeListItem *mTabGeneralSubRobot = nullptr;
    wxTreeListItem *mTabGeneralSubRobotTCP = nullptr;
    wxTreeListItem *mTabGeneralSubRobotJoints = nullptr;
    wxTreeListItem *mTabGeneralSubRobotIP = nullptr;
    wxTreeListItem *mTabGeneralSubRobotState = nullptr;

    wxTreeListItem *mTabGeneralSubCamera = nullptr;
    wxTreeListItem *mTabGeneralSubCameraIP = nullptr;
    wxTreeListItem *mTabGeneralSubCameraState = nullptr;

    wxTreeListItem *mTabGeneralSubGripper = nullptr;
    wxTreeListItem *mTabGeneralSubGripperIP = nullptr;
    wxTreeListItem *mTabGeneralSubGripperState = nullptr;

    wxTreeListItem *mTabGeneralSubDatabase = nullptr;
    wxTreeListItem *mTabGeneralSubDatabaseHost = nullptr;
    wxTreeListItem *mTabGeneralSubDatabaseUser = nullptr;
    wxTreeListItem *mTabGeneralSubDatabaseState = nullptr;
    wxTreeListItem *mTabGeneralSubDatabaseLastEntry = nullptr;

    wxTextCtrl *mTabGeneralTextCtrl = nullptr;
    wxLogTextCtrl *mTabGeneralLog = nullptr;

    // GUI elements for robot tab
    wxButton *mTabRobotConnectBtn = nullptr;
    wxButton *mTabRobotDisconnectBtn = nullptr;
    wxTextCtrl *mTabRobotIpEntryTxtCtrl = nullptr;

    // GUI elements for gripper tab
    wxButton *mTabGripperConnectBtn = nullptr;
    wxButton *mTabGripperDisconnectBtn = nullptr;
    wxTextCtrl *mTabGripperIpEntryTxtCtrl = nullptr;

    // GUI elements for camera tab
    wxButton *mTabCameraConnectBtn = nullptr;
    wxButton *mTabCameraDisconnectBtn = nullptr;
    wxTextCtrl *mTabCameraIpEntryTxtCtrl = nullptr;

    // GUI elements for database tab
    wxButton *mTabDatabaseConnectBtn = nullptr;
    wxButton *mTabDatabaseDisconnectBtn = nullptr;
    wxTextCtrl *mTabDatabaseHostEntryTxtCtrl = nullptr;
    wxTextCtrl *mTabDatabaseUserEntryTxtCtrl = nullptr;
    wxTextCtrl *mTabDatabasePasswordEntryTxtCtrl = nullptr;

    // Top menu bar
    wxMenuBar *mMenuBar = nullptr;

    // Viewing area for camera feed
    cImagePanel *mCameraPanel = nullptr;
    wxTimer mTimerCamera;

    // Viewing area for robot orientation
    cImagePanel *mRobotPanel = nullptr;

    // Pointer to the layer interface class
    std::shared_ptr<cLinker> mLinker;

    std::mutex mMtx;

    // Just a macro to enable wxWidgets events
    wxDECLARE_EVENT_TABLE();
};

#endif // CMAIN_H
