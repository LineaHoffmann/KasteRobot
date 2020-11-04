#include "cmain.hpp"

// Shell script for version auto updating
// Should maybe have some error checking inbuilt
#define VERSIONSCRIPT "\
#/bin/bash \n\
cd $PWD \n\
git describe --all --long > version.txt \n\
clear\
"

// Event -> Function binding table
// Event binding name enum is in cidbindings.hpp
wxBEGIN_EVENT_TABLE(cMain, wxFrame)
    // Timer bindings
    EVT_TIMER(ID_TIMER_VIEW1_UPDATE, cMain::OnTimerView1Update)
    //EVT_TIMER(ID_TIMER_VIEW2_UPDATE, cMain::OnTimerView2Update)
    EVT_TIMER(ID_TIMER_INFOTREE_UPDATE, cMain::OnTimerInfoUpdate)

    // Menu and button bindings go the same way
    EVT_MENU(wxID_ANY, cMain::OnButtonPress)
    EVT_BUTTON(wxID_ANY, cMain::OnButtonPress)
wxEND_EVENT_TABLE()

/**
 * @brief Builds the GUI as a wxFrame to be run by a parent wxApp
 */
cMain::cMain() : wxFrame (nullptr, wxID_ANY, "Robot Control Interface", wxDefaultPosition, wxSize(1280,1024)),
                 mTimerView1 (this, ID_TIMER_VIEW1_UPDATE),
                 mTimerInfo (this, ID_TIMER_INFOTREE_UPDATE)
{
    // Icon for the window & about box
    wxIcon icon(wxT("../resources/icon.png"), wxBITMAP_TYPE_PNG);
    SetIcon(icon);

    // Notebook area
    mNotebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER);
    // Text control area
    mTextCtrl = new wxTextCtrl(this,
                               wxID_ANY,
                               wxEmptyString,
                               wxDefaultPosition,
                               wxDefaultSize,
                               0|wxVSCROLL|wxBORDER|wxTE_READONLY | wxTE_MULTILINE | wxTE_LEFT | wxTE_CHARWRAP);
    mTextLog = new wxLogTextCtrl(mTextCtrl); // For writing to gui log system-wide, should be thread safe
    wxLog::SetActiveTarget(mTextLog);
    // Image viewing panel 1
    mPanelView1 = new cImagePanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, "View 1");
    // Image viewing panel 2
    mPanelView2 = new cImagePanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, "View 2");
    // Main grid sizer
    SetSizeHints( wxDefaultSize, wxDefaultSize );
    wxGridSizer* gridSizer;
    gridSizer = new wxGridSizer( 0, 2, 0, 0 );
    // Order dictates placement, top left - top right - bottom left - bottom right
    gridSizer->Add( mTextCtrl, 1, wxEXPAND | wxALL, 0 );
    gridSizer->Add( mPanelView1, 1, wxEXPAND | wxALL, 0 );
    gridSizer->Add( mNotebook, 1, wxEXPAND | wxALL, 0 );
    gridSizer->Add( mPanelView2, 1, wxEXPAND | wxALL, 0 );

    // Notebook contents
    wxBoxSizer *mSizerNotebookGeneral = new wxBoxSizer(wxVERTICAL);
    //wxBoxSizer *mSizerNotebookRobot = new wxBoxSizer(wxHORIZONTAL);
    //wxBoxSizer *mSizerNotebookGripper = new wxBoxSizer(wxHORIZONTAL);
    //wxBoxSizer *mSizerNotebookCamera = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *mSizerNotebookDatabase = new wxBoxSizer(wxHORIZONTAL);
    wxPanel *mNotebookGeneral = new wxPanel(mNotebook,
                                            wxID_ANY,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxTAB_TRAVERSAL | wxNO_BORDER,
                                            "General panel");
    wxPanel *mNotebookRobot = new wxPanel(mNotebook,
                                          wxID_ANY,
                                          wxDefaultPosition,
                                          wxDefaultSize,
                                          wxTAB_TRAVERSAL | wxNO_BORDER,
                                          "Robot panel");
    wxPanel *mNotebookGripper = new wxPanel(mNotebook,
                                            wxID_ANY,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxTAB_TRAVERSAL | wxNO_BORDER,
                                            "Gripper panel");
    wxPanel *mNotebookCamera = new wxPanel(mNotebook,
                                           wxID_ANY,
                                           wxDefaultPosition,
                                           wxDefaultSize,
                                           wxTAB_TRAVERSAL | wxNO_BORDER,
                                           "Camera panel");
    wxPanel *mNotebookDatabase = new wxPanel(mNotebook,
                                             wxID_ANY,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxTAB_TRAVERSAL | wxNO_BORDER,
                                             "Database panel");
    wxPanel *mNotebookTesting = new wxPanel(mNotebook,
                                             wxID_ANY,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxTAB_TRAVERSAL | wxNO_BORDER,
                                             "Testing panel");
    mNotebookGeneral->SetSizer(mSizerNotebookGeneral);
    //mNotebookRobot->SetSizer(mSizerNotebookRobot);
    //mNotebookGripper->SetSizer(mSizerNotebookGripper);
    //mNotebookCamera->SetSizer(mSizerNotebookCamera);
    mNotebookDatabase->SetSizer(mSizerNotebookDatabase);
    mNotebook->InsertPage(0,mNotebookGeneral, "General");
    mNotebook->InsertPage(1,mNotebookRobot, "Robot");
    mNotebook->InsertPage(2,mNotebookGripper, "Gripper");
    mNotebook->InsertPage(3,mNotebookCamera, "Camera");
    mNotebook->InsertPage(4,mNotebookDatabase, "Database");
    mNotebook->InsertPage(5,mNotebookTesting, "Testing");
    // Tree list creation (shows in mNotebookGeneral tab)
    mTreeList = new wxTreeListCtrl(mNotebookGeneral,
                                             wxID_ANY,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxTL_DEFAULT_STYLE);
    mTreeList->AppendColumn("Description",
                                      160,
                                      wxALIGN_LEFT,
                                      wxCOL_RESIZABLE | wxCOL_SORTABLE);
    mTreeList->AppendColumn("Data",
                                      wxCOL_WIDTH_DEFAULT,
                                      wxALIGN_RIGHT,
                                      wxCOL_RESIZABLE);
    mSizerNotebookGeneral->Add(mTreeList, wxSizerFlags(1).Expand());
    // This is where to add items to the tree
    wxTreeListItem root = mTreeList->GetRootItem();
    // Top level
    mTreeRootRobot = new wxTreeListItem(mTreeList->AppendItem(root, "Robot"));
    mTreeRootCamera = new wxTreeListItem(mTreeList->AppendItem(root, "Camera"));
    mTreeRootGripper = new wxTreeListItem(mTreeList->AppendItem(root, "Gripper"));
    mTreeRootDatabase = new wxTreeListItem(mTreeList->AppendItem(root, "Database"));
    //INDEX: Sub levels - Robot
    mTreeRobotState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "State"));
    mTreeRobotIP = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "IP"));
    mTreeRobotPort = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "Port"));
    mTreeRobotPosition = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "Position"));
    //INDEX: Sub levels - Camera
    mTreeCameraState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootCamera, "State"));
    mTreeCameraExposure = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootCamera, "Exposure Time"));
    mTreeCameraFramerate = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootCamera, "Framerate"));
    mTreeCameraCalibrationPath = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootCamera, "Calibration Path"));
    //INDEX: Sub levels - Gripper
    mTreeGripperState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootGripper, "State"));
    mTreeGripperIP = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootGripper, "IP"));
    mTreeGripperPort = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootGripper, "Port"));
    mTreeGripperWidth = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootGripper, "Width"));
    //INDEX: Sub levels - Database
    mTreeDatabaseState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "State"));
    mTreeDatabaseIP = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "IP"));
    mTreeDatabasePort = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "Port"));
    mTreeDatabaseName = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "Name"));
    mTreeDatabaseSchema = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "Schema"));

    //INDEX: Robot tab building - Buttons
    mBtnRobotConnect = new wxButton(mNotebookRobot, ID_BTN_ROBOT_CONNECT, "Connect");
    mBtnRobotDisconnect = new wxButton(mNotebookRobot, ID_BTN_ROBOT_DISCONNECT, "Disconnect");
    mBtnRobotUpdate = new wxButton(mNotebookRobot, ID_BTN_ROBOT_UPDATE, "Update");
    mBtnRobotSendCmd = new wxButton(mNotebookRobot, ID_BTN_ROBOT_SEND_CMD, "Send CMD");
    mBtnRobotSendPos = new wxButton(mNotebookRobot, ID_BTN_ROBOT_SEND_POS, "Send Pos");
    mBtnRobotSendHome = new wxButton(mNotebookRobot, ID_BTN_ROBOT_SEND_HOME, "HOME");
    mBtnRobotSendPickup = new wxButton(mNotebookRobot, ID_BTN_ROBOT_SEND_PICKUP, "PICKUP");
    //INDEX: Robot tab building - Text controls
    mTxtRobotIP = new wxTextCtrl(mNotebookRobot, wxID_ANY, "127.0.0.1");
    mTxtRobotCmd = new wxTextCtrl(mNotebookRobot, wxID_ANY, "Enter Commands here");
    mTxtRobotX = new wxTextCtrl(mNotebookRobot, wxID_ANY, "X");
    mTxtRobotY = new wxTextCtrl(mNotebookRobot, wxID_ANY, "Y");
    mTxtRobotZ = new wxTextCtrl(mNotebookRobot, wxID_ANY, "Z");
    mTxtRobotRX = new wxTextCtrl(mNotebookRobot, wxID_ANY, "RX");
    mTxtRobotRY = new wxTextCtrl(mNotebookRobot, wxID_ANY, "RY");
    mTxtRobotRZ = new wxTextCtrl(mNotebookRobot, wxID_ANY, "RZ");
    // Robot tab building - Static text
    wxStaticText *sTxtRobotX = new wxStaticText(mNotebookRobot, wxID_ANY, "X");
    wxStaticText *sTxtRobotY = new wxStaticText(mNotebookRobot, wxID_ANY, "Y");
    wxStaticText *sTxtRobotZ = new wxStaticText(mNotebookRobot, wxID_ANY, "Z");
    wxStaticText *sTxtRobotRX = new wxStaticText(mNotebookRobot, wxID_ANY, "RX");
    wxStaticText *sTxtRobotRY = new wxStaticText(mNotebookRobot, wxID_ANY, "RY");
    wxStaticText *sTxtRobotRZ = new wxStaticText(mNotebookRobot, wxID_ANY, "RZ");
    // Robot tab building - Static bitmap
    mBmpRobotStatus = new wxStaticBitmap(mNotebookRobot, wxID_ANY, GetIcon());
    mBmpRobotStatus->SetBackgroundColour(wxColor(255,0,0));
    // Robot tab building - GridBagSizer setup
    wxGridBagSizer *mSizerNotebookRobot = new wxGridBagSizer(0, 0);
    mSizerNotebookRobot->SetFlexibleDirection(wxBOTH);
    mSizerNotebookRobot->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    mSizerNotebookRobot->Add(mBtnRobotConnect, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotDisconnect, wxGBPosition(1, 1), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotUpdate, wxGBPosition(1, 2), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotSendCmd, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotSendPos, wxGBPosition(3, 0), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotSendHome, wxGBPosition(4, 0), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotSendPickup, wxGBPosition(5, 0), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mTxtRobotIP, wxGBPosition(1, 3), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookRobot->Add(mTxtRobotCmd, wxGBPosition(0, 1), wxGBSpan(1, 4), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookRobot->Add(mTxtRobotX, wxGBPosition(2, 2), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mTxtRobotY, wxGBPosition(3, 2), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mTxtRobotZ, wxGBPosition(4, 2), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mTxtRobotRX, wxGBPosition(2, 4), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mTxtRobotRY, wxGBPosition(3, 4), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mTxtRobotRZ, wxGBPosition(4, 4), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(sTxtRobotX, wxGBPosition(2, 1), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);
    mSizerNotebookRobot->Add(sTxtRobotY, wxGBPosition(3, 1), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);
    mSizerNotebookRobot->Add(sTxtRobotZ, wxGBPosition(4, 1), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);
    mSizerNotebookRobot->Add(sTxtRobotRX, wxGBPosition(2, 3), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);
    mSizerNotebookRobot->Add(sTxtRobotRY, wxGBPosition(3, 3), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);
    mSizerNotebookRobot->Add(sTxtRobotRZ, wxGBPosition(4, 3), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);
    mSizerNotebookRobot->Add( mBmpRobotStatus, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
    for (uint32_t i = 0; i < 5; i++) {
        mSizerNotebookRobot->AddGrowableCol(i);
        mSizerNotebookRobot->AddGrowableRow(i);
    }
    mNotebookRobot->SetSizer(mSizerNotebookRobot);
    mNotebookRobot->Layout();
    // Robot tab building - Tooltips
    mBtnRobotConnect->SetToolTip("Connect to robot using the IP field");
    mBtnRobotDisconnect->SetToolTip("Disconnect from the robot");
    mBtnRobotUpdate->SetToolTip("Update connection using the IP field");
    mBtnRobotSendCmd->SetToolTip("Send the command entered in the CMD field to the robot");
    mBtnRobotSendPos->SetToolTip("Send the position entered in the X/Y/Z/RX/RY/RZ fields to the robot");
    mBtnRobotSendHome->SetToolTip("Send the home position to the robot");

    // Camera tab building - Buttons
    mBtnCameraStart = new wxButton(mNotebookCamera, ID_BTN_CAMERA_START, "Start");
    mBtnCameraStop = new wxButton(mNotebookCamera, ID_BTN_CAMERA_STOP, "Stop");
    mBtnCameraRecalibrate = new wxButton(mNotebookCamera, ID_BTN_CAMERA_RECALIBRATE, "Recalibrate");
    mBtnCameraFindBall = new wxButton(mNotebookCamera, ID_BTN_CAMERA_FINDBALL, "start/stop ballseeker");
    mBtnCameraCutOut = new wxButton(mNotebookCamera, ID_BTN_CAMERA_CUT_TABLE, "CutOutTable");
    mBtnCameraDetectorSettings = new wxButton(mNotebookCamera, ID_BTN_CAMERA_LOAD_DETECTOR_SETTINGS, "Set Detector settings");
    // Camera tab building - Text controls
    mTxtCameraExposure = new wxTextCtrl(mNotebookCamera, wxID_ANY, "5000");
    mTxtCameraFramerate = new wxTextCtrl(mNotebookCamera, wxID_ANY, "30");
    mTxtCameraCalibrationPath = new wxTextCtrl(mNotebookCamera, wxID_ANY, "../resources/pylonimgs/*.bmp");
    mTxtCameraHue = new wxTextCtrl(mNotebookCamera, wxID_ANY, "10");
    mTxtCameraHueDelta = new wxTextCtrl(mNotebookCamera, wxID_ANY, "20");
    mTxtCameraBallMin = new wxTextCtrl(mNotebookCamera, wxID_ANY, "1.7");
    mTxtCameraBallMax = new wxTextCtrl(mNotebookCamera, wxID_ANY, "2.3");
    // Camera tab building - Static bitmap
    mBmpCameraStatus = new wxStaticBitmap(mNotebookCamera, wxID_ANY, GetIcon());
    mBmpCameraStatus->SetBackgroundColour(wxColor(255,0,0));
    // Camera tab building - GridBagSizer setup
    wxGridBagSizer *mSizerNotebookCamera = new wxGridBagSizer(0, 0);
    mSizerNotebookCamera->SetFlexibleDirection( wxBOTH );
    mSizerNotebookCamera->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    mSizerNotebookCamera->Add( mBtnCameraStart, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraStop, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraRecalibrate, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraFindBall, wxGBPosition( 3, 0 ), wxGBSpan( 1, 4 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraCutOut, wxGBPosition( 3, 4 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraDetectorSettings, wxGBPosition( 4, 4 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraCalibrationPath, wxGBPosition( 2, 0 ), wxGBSpan( 1, 3 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraExposure, wxGBPosition( 2, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraFramerate, wxGBPosition( 2, 4 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraHue, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraHueDelta, wxGBPosition( 4, 1 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraBallMin, wxGBPosition( 4, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraBallMax, wxGBPosition( 4, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBmpCameraStatus, wxGBPosition( 1, 4 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( 0, 0, wxGBPosition( 0, 0 ), wxGBSpan( 1, 5 ), wxALL|wxEXPAND, 5 );
    for (uint32_t i = 0; i < 5; i++) {
        mSizerNotebookCamera->AddGrowableCol(i);
        mSizerNotebookCamera->AddGrowableRow(i);
    }
    mNotebookCamera->SetSizer( mSizerNotebookCamera );
    mNotebookCamera->Layout();
    mTxtCameraCalibrationPath->SetToolTip("Set calibration path");
    mTxtCameraExposure->SetToolTip("set exposure");
    mTxtCameraFramerate->SetToolTip("set framerate");
    mTxtCameraHue->SetToolTip("set detection hue");
    mTxtCameraHueDelta->SetToolTip("set hue delta (+-value)");
    mTxtCameraBallMin->SetToolTip("set minimum ball size");
    mTxtCameraBallMax->SetToolTip("set maximum ball size");

    // Gripper tab building - Buttons
    mBtnGripperConnect = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_CONNECT, "Connect");
    mBtnGripperDisconnect = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_DISCONNECT, "Disconnect");
    mBtnGripperUpdate = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_UPDATE, "Update");
    mBtnGripperRelease = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_RELEASE, "Release");
    mBtnGripperGrip = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_GRIP, "Grip");
    mBtnGripperSendCmd = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_SEND_CMD, "Send cmd");
    mBtnGripperHome = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_HOME, "Home");
    // Gripper tab building - Text controls
    mTxtGripperIP = new wxTextCtrl(mNotebookGripper, wxID_ANY, "IP");
    mTxtGripperPort = new wxTextCtrl(mNotebookGripper, wxID_ANY, "Port");
    mTxtGripperCmd = new wxTextCtrl(mNotebookGripper, wxID_ANY, "Command");
    // Gripper tab building - Static bitmap
    mBmpGripperStatus = new wxStaticBitmap(mNotebookGripper, wxID_ANY, GetIcon());
    mBmpGripperStatus->SetBackgroundColour(wxColor(255,0,0));
    // Gripper tab building - GridBagSizer setup
    wxGridBagSizer *mSizerNotebookGripper = new wxGridBagSizer(0, 0);
    mSizerNotebookGripper->SetFlexibleDirection( wxBOTH );
    mSizerNotebookGripper->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    mSizerNotebookGripper->Add( mBtnGripperConnect, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mBtnGripperDisconnect, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mBtnGripperUpdate, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mBtnGripperRelease, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mBtnGripperGrip, wxGBPosition( 3, 1 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mBtnGripperSendCmd, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );    
    mSizerNotebookGripper->Add( mBtnGripperHome, wxGBPosition( 3, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mTxtGripperIP, wxGBPosition( 1, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mTxtGripperPort, wxGBPosition( 1, 4 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mTxtGripperCmd, wxGBPosition( 4, 1 ), wxGBSpan( 1, 4 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mBmpGripperStatus, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( 0, 0, wxGBPosition( 0, 0 ), wxGBSpan( 1, 5 ), wxALL|wxEXPAND, 5 );
    for (uint32_t i = 0; i < 5; i++) {
        mSizerNotebookGripper->AddGrowableCol(i);
        mSizerNotebookGripper->AddGrowableRow(i);
    }
    mNotebookGripper->SetSizer(mSizerNotebookGripper);
    mNotebookGripper->Layout();

    // Database tab building
    mBtnDatabaseConnect = new wxButton(mNotebookDatabase, ID_BTN_GRIPPER_CONNECT, "Connect");
    mBtnDatabaseDisconnect = new wxButton(mNotebookDatabase, ID_BTN_GRIPPER_DISCONNECT, "Disconnect");
    mSizerNotebookDatabase->Add(mBtnDatabaseConnect);
    mSizerNotebookDatabase->Add(mBtnDatabaseDisconnect);
    mTxtDatabaseIP = new wxTextCtrl(mNotebookDatabase, wxID_ANY, "IP");
    mTxtDatabasePort = new wxTextCtrl(mNotebookDatabase, wxID_ANY, "Port");
    mTxtDatabaseUser = new wxTextCtrl(mNotebookDatabase, wxID_ANY, "User");
    mTxtDatabaseSchema = new wxTextCtrl(mNotebookDatabase, wxID_ANY, "Schema");
    mTxtDatabasePassword = new wxTextCtrl(mNotebookDatabase, wxID_ANY, "Password");
    mSizerNotebookDatabase->Add(mTxtDatabaseIP);
    mSizerNotebookDatabase->Add(mTxtDatabasePort);
    mSizerNotebookDatabase->Add(mTxtDatabaseUser);
    mSizerNotebookDatabase->Add(mTxtDatabaseSchema);
    mSizerNotebookDatabase->Add(mTxtDatabasePassword);

    // Testing tab building - Buttons
    mBtnTestMathXYZtoVAA = new wxButton(mNotebookTesting, ID_BTN_TESTING_XYZ_VVA, "XYZ->VAA");
    mBtnTestDETECTnPICK = new wxButton(mNotebookTesting, ID_BTN_TESTING_DETECT_N_PICK, "Detect and pick");
    // Testing tab building - Text Controls
    mTxtTestMathInX = new wxTextCtrl(mNotebookTesting, wxID_ANY, "X");
    mTxtTestMathInY = new wxTextCtrl(mNotebookTesting, wxID_ANY, "Y");
    mTxtTestMathInZ = new wxTextCtrl(mNotebookTesting, wxID_ANY, "Z");
    mTxtTestMathInAngleLowLim = new wxTextCtrl(mNotebookTesting, wxID_ANY, "Low Angle");
    mTxtTestMathInAngleHighLim = new wxTextCtrl(mNotebookTesting, wxID_ANY, "High Angle");
    mTxtTestMathInVelocityLowLim = new wxTextCtrl(mNotebookTesting, wxID_ANY, "Low Speed");
    mTxtTestMathInVelocityHighLim = new wxTextCtrl(mNotebookTesting, wxID_ANY, "High Speed");
    // Testing tab building - GridBagSizer setup
    wxGridBagSizer *mSizerNotebookTesting = new wxGridBagSizer(0, 0);
    mSizerNotebookTesting->SetFlexibleDirection( wxBOTH );
    mSizerNotebookTesting->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    mSizerNotebookTesting->Add( mBtnTestMathXYZtoVAA, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mBtnTestDETECTnPICK, wxGBPosition( 0, 5 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );

    mSizerNotebookTesting->Add( mTxtTestMathInX, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInY, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInZ, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInAngleLowLim, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInAngleHighLim, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInVelocityLowLim, wxGBPosition( 1, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInVelocityHighLim, wxGBPosition( 2, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mNotebookTesting->SetSizer(mSizerNotebookTesting);
    mNotebookTesting->Layout();
    // Testing tab building - Tooltips
    mBtnTestMathXYZtoVAA->SetToolTip("Try calculating from distance XYZ to velocity and XY/(XY)Z-directions");
    mBtnTestDETECTnPICK->SetToolTip("Will try and find a ball, and then go at try to pick it up (not done)");

    // About box creation
    mAboutBox = new wxAboutDialogInfo;
    mAboutBox->SetIcon(icon);
    mAboutBox->AddDeveloper("Jacob Warrer");
    mAboutBox->AddDeveloper("Jonas Lorentzen");
    mAboutBox->AddDeveloper("Linea Hoffmann");
    mAboutBox->AddDeveloper("Mathias Halbro");
    mAboutBox->AddDeveloper("Mikkel Joergensen");
    mAboutBox->AddDeveloper("Soeren Pedersen");
    mAboutBox->SetName("Robot Thrower");
    mAboutBox->SetDescription("3rd Semester Project in Robotics\n"
                              "University of Southern Denmark");
    system(VERSIONSCRIPT);  // Creates a version.txt file in the build dir
                            // NOTE: Doesn't have error handling, will crash if it fails
    std::ifstream istr("version.txt");  // Reads the version.txt
    std::ostringstream ostr; ostr << istr.rdbuf();
    std::string s = ostr.str(); s.erase(s.begin(), s.begin() + s.find("/") + 1);
    if (!s.empty()) mAboutBox->SetVersion(s.c_str());
    else mAboutBox->SetVersion("VERSION SET FAILED");

    // Menu bar creation
    mMenuBar = new wxMenuBar();
    SetMenuBar(mMenuBar);
    wxMenu *menuFile = new wxMenu();
    menuFile->Append(ID_MENU_SAVE_LOG, "Save Log", "Save Log");
    menuFile->Append(ID_MENU_SAVE_SNAPSHOT, "Save Snapshot", "Save Snapshot");
    menuFile->Append(ID_MENU_EXIT, "Exit", "Exit");
    menuFile->Append(ID_MENU_ABOUT, "About", "About");
    mMenuBar->Append(menuFile,"File");

    // Status bar creation
    mStatusBar = new wxStatusBar(this, wxID_ANY, wxSTB_DEFAULT_STYLE, "Status bar");
    mStatusBar->PushStatusText("Starting ..", 0);
    mStatusBar->SetFieldsCount(3);
    SetStatusBar(mStatusBar);

    // Final details and centering
    SetSizer(gridSizer);
    Layout();
    Centre(wxBOTH);

    // Starting timers
    mTimerView1.Start(40);
    mTimerInfo.Start(1000);

    // Starting with expanded treelist
    mTreeList->Expand(*mTreeRootRobot);
    mTreeList->Expand(*mTreeRootCamera);
    mTreeList->Expand(*mTreeRootGripper);
    mTreeList->Expand(*mTreeRootDatabase);
}
cMain::~cMain()
{
    mTimerView1.Stop();
}
void cMain::pushStrToStatus(std::string &msg)
{
    std::lock_guard<std::mutex> lock(mMtx);
    // If called during Close(), the push may fail because mStatusBar is destructing elsewhere
    if (mStatusBar != NULL && mStatusBar->GetFieldsCount() == 3)
        mStatusBar->PushStatusText(msg.c_str(), 2);
}
void cMain::popStrFromStatus()
{
    std::lock_guard<std::mutex> lock(mMtx);
    // If called during Close(), the pop may fail because mStatusBar is destructing elsewhere
    if (mStatusBar != NULL && mStatusBar->GetFieldsCount() == 3)
        mStatusBar->PopStatusText(2);
}
void cMain::setLogicControllerPointer(std::shared_ptr<xController> controller)
{
    mController = controller;
}
void cMain::OnTimerView1Update(wxTimerEvent &evt)
{
    // Check if a new image is available
    // If it is, copy to local variable and then into View 1
    if (!mController || !mController->hasNewImage()) return;
    cv::Mat input = mController->getImage(); // Returns a full clone
    cv::Mat im2;
    if (input.channels() == 1) {
        // Probably grayscale
        cv::cvtColor(input, im2, cv::COLOR_GRAY2RGB);
    } else if (input.channels() == 4) {
        // Probably BGR with alpha channel
        cv::cvtColor(input, im2, cv::COLOR_BGRA2RGB);
    } else {
        // Probably BGR without alpha channel
        cv::cvtColor(input, im2, cv::COLOR_BGR2RGB);
    }
    long imgSize = im2.rows * im2.cols * im2.channels();
    wxImage output(im2.cols, im2.rows, imgSize);
    unsigned char* source = im2.data;
    unsigned char* destination = output.GetData();
    for (long i = 0; i < imgSize; i++) {
        destination[i] = source[i];
    }
    mPanelView1->setNewImage(output);
    evt.Skip();
}
void cMain::OnTimerInfoUpdate(wxTimerEvent &evt)
{
    // Updating event for refreshing the info tree
    // TODO: Write this
    // Collect info from all available object through
    // the logic controller, in a thread safe manner

    // POSIX resource use
    struct rusage use;
    if (getrusage(RUSAGE_SELF, &use) == 0) {
        std::string s = "Current Resource Use [MB]: ";
        s.append(std::to_string(use.ru_maxrss / 1048576.0f));
        pushStrToStatus(s);
    }
    evt.Skip();
}
void cMain::OnButtonPress(wxCommandEvent &evt) {
    // Switchcase to the appropriate response
    switch (evt.GetId()) {
    case ID_MENU_SAVE_LOG:
        xTry([&] {mController->guiButtonPressed(ID_MENU_SAVE_LOG);});
        // TODO: Pull current run entries from database
        //       and dump to some file?
        break;
    case ID_MENU_SAVE_SNAPSHOT:
    {
        wxCoord windowWidth, windowHeight;
        wxClientDC clientDC(this);
        clientDC.GetSize(&windowWidth, &windowHeight);
        wxBitmap bitmap(windowWidth, windowHeight, -1);
        wxMemoryDC memoryDC;
        memoryDC.SelectObject(bitmap);
        memoryDC.Blit(0, 0, windowWidth, windowHeight, &clientDC, 0, 0);
        memoryDC.SelectObject(wxNullBitmap);
        time_t now = time(0);
        char* dt = std::ctime(&now);
        std::stringstream path;
        path << "../snapshots/snap " << dt << ".png";
        bitmap.SaveFile(path.str().c_str(), wxBITMAP_TYPE_PNG);
        std::string s = "Snapshot saved: "; s.append(path.str());
        logstd(s.c_str());
    }
        break;
    case ID_MENU_EXIT:
        Close(true);
        evt.Skip();
        return;
    case ID_MENU_ABOUT:
        wxAboutBox(*mAboutBox);
        break;
    case ID_BTN_ROBOT_CONNECT:
        {
        std::string ip;
        ip = mTxtRobotIP->GetValue().ToStdString();
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_CONNECT, ip);});
        }
        break;
    case ID_BTN_ROBOT_DISCONNECT:
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_DISCONNECT);});
        break;
    case ID_BTN_ROBOT_UPDATE:
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_UPDATE);});
        break;
    case ID_BTN_ROBOT_SEND_CMD:
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_SEND_CMD);});
        break;
    case ID_BTN_ROBOT_SEND_POS:
        {
        std::vector<double> q;
        double d;
        mTxtRobotX->GetValue().ToDouble(&d);
        q.push_back(d);
        mTxtRobotY->GetValue().ToDouble(&d);
        q.push_back(d);
        mTxtRobotZ->GetValue().ToDouble(&d);
        q.push_back(d);
        mTxtRobotRX->GetValue().ToDouble(&d);
        q.push_back(d);
        mTxtRobotRY->GetValue().ToDouble(&d);
        q.push_back(d);
        mTxtRobotRZ->GetValue().ToDouble(&d);
        q.push_back(d);
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_SEND_POS, q);});
        }
        break;
    case ID_BTN_ROBOT_SEND_HOME:
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_SEND_HOME);});
        break;
    case ID_BTN_ROBOT_SEND_PICKUP:
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_SEND_PICKUP);});
        break;
    case ID_BTN_GRIPPER_CONNECT:
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_CONNECT);});
        break;
    case ID_BTN_GRIPPER_DISCONNECT:
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_DISCONNECT);});
        break;
    case ID_BTN_GRIPPER_UPDATE:
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_UPDATE);});
        break;
    case ID_BTN_GRIPPER_RELEASE:
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_RELEASE);});
        break;
    case ID_BTN_GRIPPER_GRIP:
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_GRIP);});
        break;
    case ID_BTN_GRIPPER_SEND_CMD:
    {
        std::string cmd = mTxtGripperCmd->GetValue().ToStdString();
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_SEND_CMD, cmd);});
    }
        break;
    case ID_BTN_GRIPPER_HOME:
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_HOME);});
        break;
    case ID_BTN_CAMERA_START:
    {
        double expo;
        unsigned long fps;
        std::pair<double, uint64_t> data;
        mTxtCameraExposure->GetValue().ToDouble(&expo);
        mTxtCameraFramerate->GetValue().ToULong(&fps);
        data = {expo, static_cast<uint64_t>(fps)};
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_START, data);});
    }
        break;
    case ID_BTN_CAMERA_STOP:
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_STOP);});
        break;
    case ID_BTN_CAMERA_FINDBALL:
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_FINDBALL);});
        break;
    case ID_BTN_CAMERA_RECALIBRATE:
    {
        std::string path = mTxtCameraCalibrationPath->GetValue().ToStdString();
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_RECALIBRATE, path);});
    }
        break;
    case ID_BTN_CAMERA_CUT_TABLE:

        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_CUT_TABLE);});
        break;
    case ID_BTN_CAMERA_LOAD_DETECTOR_SETTINGS:
    {
        long hue, hueDelta;
        double min, max;
        mTxtCameraBallMax->GetValue().ToDouble(&max);
        mTxtCameraBallMin->GetValue().ToDouble(&min);
        mTxtCameraHue->GetValue().ToLong(&hue);
        mTxtCameraHueDelta->GetValue().ToLong(&hueDelta);
        std::tuple<long, long, float, float> data(hue, hueDelta, min, max);
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_LOAD_DETECTOR_SETTINGS, data);});
    }
        break;
    case ID_BTN_DATABASE_CONNECT:
        xTry([&] {mController->guiButtonPressed(ID_BTN_DATABASE_CONNECT);});
        break;
    case ID_BTN_DATABASE_DISCONNECT:
        xTry([&] {mController->guiButtonPressed(ID_BTN_DATABASE_DISCONNECT);});
        break;
    case ID_BTN_DATABASE_UPDATE:
        xTry([&] {mController->guiButtonPressed(ID_BTN_DATABASE_UPDATE);});
        break;
    case ID_BTN_TESTING_XYZ_VVA:
    {
        // Load data from testing fields of this math function
        double x, y, z, alow, ahigh, vlow, vhigh;
        mTxtTestMathInX->GetValue().ToDouble(&x);
        mTxtTestMathInY->GetValue().ToDouble(&y);
        mTxtTestMathInZ->GetValue().ToDouble(&z);
        mTxtTestMathInAngleLowLim->GetValue().ToDouble(&alow);
        mTxtTestMathInAngleHighLim->GetValue().ToDouble(&ahigh);
        mTxtTestMathInVelocityLowLim->GetValue().ToDouble(&vlow);
        mTxtTestMathInVelocityHighLim->GetValue().ToDouble(&vhigh);
        std::array<double, 7> data {x, y, z, alow, ahigh, vlow, vhigh};
        xTry( [&] {
            mController->guiButtonPressed<std::array<double, 7>>(ID_BTN_TESTING_XYZ_VVA, data);
        });
    }
        break;
    default:
        logerr("An event fell through the button handler!");
        break;
    }
    evt.Skip();
    return;
}
