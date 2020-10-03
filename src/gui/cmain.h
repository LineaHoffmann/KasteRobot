#ifndef CMAIN_H
#define CMAIN_H

#include <mutex>
#include <sstream>

#include "cimagepanel.h"
#include "clinker.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/sizer.h"
    #include "wx/textctrl.h"
#endif

#include "wx/notebook.h"
#include "wx/treelist.h"
#include "wx/aboutdlg.h"
#include "wx/artprov.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "sample.xpm"
#endif

class cMain : public wxFrame
{
public:
    cMain();
    ~cMain();

    // GUI event handler functions, linked in top of cMain.cpp
    // Main window top bar functions
    void OnMenuSaveLog(wxCommandEvent &evt);
    void OnMenuSaveSnap(wxCommandEvent &evt);
    void OnMenuExit(wxCommandEvent &evt);
    void OnMenuAbout(wxCommandEvent &evt);

    // Pointer to the logic layer interface class
    cLinker *mLinker = nullptr;
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

    // Viewing area for robot orientation
    cImagePanel *mRobotPanel = nullptr;

    // Just a macro to enable wxWidgets events
    wxDECLARE_EVENT_TABLE();
};

#endif // CMAIN_H
