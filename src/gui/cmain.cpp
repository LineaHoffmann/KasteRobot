#include "cmain.hpp"

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
    mPanelView1 = new cImagePanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    // Image viewing panel 2
    mPanelView2 = new cImagePanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
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
    wxColour LIGHT_BLUE = wxColor(84,88,94);
    mNotebook->SetBackgroundColour(LIGHT_BLUE);
    mNotebookGeneral->SetBackgroundColour(LIGHT_BLUE);
    mNotebookRobot->SetBackgroundColour(LIGHT_BLUE);
    mNotebookGripper->SetBackgroundColour(LIGHT_BLUE);
    mNotebookCamera->SetBackgroundColour(LIGHT_BLUE);
    mNotebookDatabase->SetBackgroundColour(LIGHT_BLUE);
    mNotebookTesting->SetBackgroundColour(LIGHT_BLUE);
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
    // Sub levels - Robot
    mTreeRobotState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "State"));
    mTreeRobotIP = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "IP"));
    mTreeRobotPort = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "Port"));
    mTreeRobotPosition = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "Position"));
    // Sub levels - Camera
    mTreeCameraState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootCamera, "State"));
    mTreeCameraExposure = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootCamera, "Exposure Time"));
    mTreeCameraFramerate = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootCamera, "Framerate"));
    mTreeCameraCalibrationPath = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootCamera, "Calibration Path"));
    // Sub levels - Gripper
    mTreeGripperState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootGripper, "State"));
    mTreeGripperIP = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootGripper, "IP"));
    mTreeGripperPort = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootGripper, "Port"));
    mTreeGripperWidth = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootGripper, "Width"));
    // Sub levels - Database
    mTreeDatabaseState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "State"));
    mTreeDatabaseIP = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "IP"));
    mTreeDatabasePort = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "Port"));
    mTreeDatabaseName = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "Name"));
    mTreeDatabaseSchema = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootDatabase, "Schema"));

    // Robot tab building - Buttons
    mBtnRobotConnect = new wxButton(mNotebookRobot, ID_BTN_ROBOT_CONNECT, "Connect");
    mBtnRobotDisconnect = new wxButton(mNotebookRobot, ID_BTN_ROBOT_DISCONNECT, "Disconnect");
    mBtnRobotUpdate = new wxButton(mNotebookRobot, ID_BTN_ROBOT_UPDATE, "Update");
    mBtnRobotSendCmd = new wxButton(mNotebookRobot, ID_BTN_ROBOT_SEND_CMD, "Send CMD");
    mBtnRobotSendPos = new wxButton(mNotebookRobot, ID_BTN_ROBOT_SEND_POS, "Send Pos");
    // Robot tab building - Text controls
    mTxtRobotIP = new wxTextCtrl(mNotebookRobot, wxID_ANY, "IP");
    mTxtRobotPort = new wxTextCtrl(mNotebookRobot, wxID_ANY, "Port");
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
    // Camera tab building - Static bitmap
    mBmpRobotStatus = new wxStaticBitmap(mNotebookRobot, wxID_ANY, GetIcon());
    mBmpRobotStatus->SetBackgroundColour(wxColor(255,0,0));
    // Robot tab building - GridBagSizer setup
    wxGridBagSizer *mSizerNotebookRobot = new wxGridBagSizer(0, 0);
    mSizerNotebookRobot->SetFlexibleDirection(wxBOTH);
    mSizerNotebookRobot->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    mSizerNotebookRobot->Add(mBtnRobotConnect, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotDisconnect, wxGBPosition(0, 1), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotUpdate, wxGBPosition(0, 2), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotSendCmd, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mBtnRobotSendPos, wxGBPosition(3, 0), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5);
    mSizerNotebookRobot->Add(mTxtRobotIP, wxGBPosition(0, 3), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookRobot->Add(mTxtRobotPort, wxGBPosition(0, 4), wxGBSpan(1, 1), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookRobot->Add(mTxtRobotCmd, wxGBPosition(1, 1), wxGBSpan(1, 4), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
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

    // Camera  tab building - Buttons
    mBtnCameraConnect = new wxButton(mNotebookCamera, ID_BTN_CAMERA_CONNECT, "Connect");
    mBtnCameraDisconnect = new wxButton(mNotebookCamera, ID_BTN_CAMERA_DISCONNECT, "Disconnect");
    mBtnCameraRecalibrate = new wxButton(mNotebookCamera, ID_BTN_CAMERA_RECALIBRATE, "Recalibrate");
    mBtnCameraSetExposure = new wxButton(mNotebookCamera, ID_BTN_CAMERA_SET_EXPOSURE, "Set Exposure");
    mBtnCameraSetFramerate = new wxButton(mNotebookCamera, ID_BTN_CAMERA_SET_FRAMERATE, "Set Framerate");
    mBtnCameraSetCalibrationPath = new wxButton(mNotebookCamera, ID_BTN_CAMERA_SET_CAL_PATH, "Set Cal. Path");
    // Camera tab building - Text controls
    mTxtCameraExposure = new wxTextCtrl(mNotebookCamera, wxID_ANY, "Exposure Time");
    mTxtCameraFramerate = new wxTextCtrl(mNotebookCamera, wxID_ANY, "Framerate");
    mTxtCameraCalibrationPath = new wxTextCtrl(mNotebookCamera, wxID_ANY, "Calibration path");
    // Camera tab building - Static bitmap
    mBmpCameraStatus = new wxStaticBitmap(mNotebookCamera, wxID_ANY, GetIcon());
    mBmpCameraStatus->SetBackgroundColour(wxColor(255,0,0));
    // Camera tab building - GridBagSizer setup
    wxGridBagSizer *mSizerNotebookCamera = new wxGridBagSizer(0, 0);
    mSizerNotebookCamera->SetFlexibleDirection( wxBOTH );
    mSizerNotebookCamera->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    mSizerNotebookCamera->Add( mBtnCameraConnect, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraDisconnect, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraRecalibrate, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraSetExposure, wxGBPosition( 1, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraSetFramerate, wxGBPosition( 1, 4 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBtnCameraSetCalibrationPath, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraExposure, wxGBPosition( 2, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraFramerate, wxGBPosition( 2, 4 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mTxtCameraCalibrationPath, wxGBPosition( 3, 1 ), wxGBSpan( 1, 3 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( mBmpCameraStatus, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( 0, 0, wxGBPosition( 0, 0 ), wxGBSpan( 1, 5 ), wxALL|wxEXPAND, 5 );
    mSizerNotebookCamera->Add( 0, 0, wxGBPosition( 4, 0 ), wxGBSpan( 1, 5 ), wxALL|wxEXPAND, 5 );
    for (uint32_t i = 0; i < 5; i++) {
        mSizerNotebookCamera->AddGrowableCol(i);
        mSizerNotebookCamera->AddGrowableRow(i);
    }
    mNotebookCamera->SetSizer( mSizerNotebookCamera );
    mNotebookCamera->Layout();

    // Gripper tab building - Buttons
    mBtnGripperConnect = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_CONNECT, "Connect");
    mBtnGripperDisconnect = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_DISCONNECT, "Disconnect");
    mBtnGripperUpdate = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_UPDATE, "Update");
    mBtnGripperOpen = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_OPEN, "Open");
    mBtnGripperClose = new wxButton(mNotebookGripper, ID_BTN_GRIPPER_CLOSE, "Close");
    // Gripper tab building - Text controls
    mTxtGripperIP = new wxTextCtrl(mNotebookGripper, wxID_ANY, "IP");
    mTxtGripperPort = new wxTextCtrl(mNotebookGripper, wxID_ANY, "Port");
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
    mSizerNotebookGripper->Add( mBtnGripperOpen, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mBtnGripperClose, wxGBPosition( 3, 1 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mTxtGripperIP, wxGBPosition( 1, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mTxtGripperPort, wxGBPosition( 1, 4 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( mBmpGripperStatus, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( 0, 0, wxGBPosition( 0, 0 ), wxGBSpan( 1, 5 ), wxALL|wxEXPAND, 5 );
    mSizerNotebookGripper->Add( 0, 0, wxGBPosition( 4, 0 ), wxGBSpan( 1, 5 ), wxALL|wxEXPAND, 5 );
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
    mSizerNotebookTesting->Add( mTxtTestMathInX, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInY, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInZ, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInAngleLowLim, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInAngleHighLim, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInVelocityLowLim, wxGBPosition( 1, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mSizerNotebookTesting->Add( mTxtTestMathInVelocityHighLim, wxGBPosition( 2, 3 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER|wxEXPAND, 5 );
    mNotebookTesting->SetSizer(mSizerNotebookTesting);
    mNotebookTesting->Layout();

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
    mTimerView1.Start(500);
    mTimerInfo.Start(1000);

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
        logstd("Menu->Save Log clicked");
        xTry([&] {mController->guiButtonPressed(ID_MENU_SAVE_LOG);});
        // TODO: Pull current run entries from database
        //       and dump to some file
        evt.Skip();
        break;
    case ID_MENU_SAVE_SNAPSHOT:
        logstd("Menu->Save Snapshot clicked");
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
        evt.Skip();
    }
        break;
    case ID_MENU_EXIT:
        // Sending to cout because the application will die now
        std::cout << "Menu->Exit clicked" << std::endl;
        Close(true);
        evt.Skip();
        break;
    case ID_MENU_ABOUT:
        logstd("Menu->About clicked");
        wxAboutBox(*mAboutBox);
        evt.Skip();
        break;
    case ID_BTN_ROBOT_CONNECT:
        logstd("Robot->Connect clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_CONNECT);});
        evt.Skip();
        break;
    case ID_BTN_ROBOT_DISCONNECT:
        logstd("Robot->Disconnect clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_DISCONNECT);});
        evt.Skip();
        break;
    case ID_BTN_ROBOT_UPDATE:
        logstd("Robot->Update clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_UPDATE);});
        evt.Skip();
        break;
    case ID_BTN_ROBOT_SEND_CMD:
        logstd("Robot->Send Command clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_SEND_CMD);});
        evt.Skip();
        break;
    case ID_BTN_ROBOT_SEND_POS:
        logstd("Robot->Send Position clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_ROBOT_SEND_POS);});
        evt.Skip();
        break;
    case ID_BTN_GRIPPER_CONNECT:
        logstd("Gripper->Connect clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_CONNECT);});
        evt.Skip();
        break;
    case ID_BTN_GRIPPER_DISCONNECT:
        logstd("Gripper->Disconnect clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_DISCONNECT);});
        evt.Skip();
        break;
    case ID_BTN_GRIPPER_UPDATE:
        logstd("Gripper->Update clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_UPDATE);});
        evt.Skip();
        break;
    case ID_BTN_GRIPPER_OPEN:
        logstd("Gripper->Open clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_OPEN);});
        evt.Skip();
        break;
    case ID_BTN_GRIPPER_CLOSE:
        logstd("Gripper->Close clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_GRIPPER_CLOSE);});
        evt.Skip();
        break;
    case ID_BTN_CAMERA_CONNECT:
        logstd("Camera->Connect clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_CONNECT);});
        evt.Skip();
        break;
    case ID_BTN_CAMERA_DISCONNECT:
        logstd("Camera->Disconnect clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_DISCONNECT);});
        evt.Skip();
        break;
    case ID_BTN_CAMERA_RECALIBRATE:
        logstd("Camera->Recalibrate clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_RECALIBRATE);});
        evt.Skip();
        break;
    case ID_BTN_CAMERA_SET_EXPOSURE:
        logstd("Camera->Set Exposure clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_SET_EXPOSURE);});
        evt.Skip();
        break;
    case ID_BTN_CAMERA_SET_FRAMERATE:
        logstd("Camera->Set Framerate clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_SET_FRAMERATE);});
        evt.Skip();
        break;
    case ID_BTN_CAMERA_SET_CAL_PATH:
        logstd("Camera->Set Calibration Path clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_CAMERA_SET_CAL_PATH);});
        evt.Skip();
        break;
    case ID_BTN_DATABASE_CONNECT:
        logstd("Database->Connect clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_DATABASE_CONNECT);});
        evt.Skip();
        break;
    case ID_BTN_DATABASE_DISCONNECT:
        logstd("Database->Disconnect clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_DATABASE_DISCONNECT);});
        evt.Skip();
        break;
    case ID_BTN_DATABASE_UPDATE:
        logstd("Database->Update clicked");
        xTry([&] {mController->guiButtonPressed(ID_BTN_DATABASE_UPDATE);});
        evt.Skip();
        break;
    case ID_BTN_TESTING_XYZ_VVA:
    {
        logstd("Testing->XYZ_VVA clicked");
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
            mController->guiButtonPressed(ID_BTN_TESTING_XYZ_VVA, data);
        });
    }
        evt.Skip();
        break;
    default:
        logerr("An event fell through the button handler!");
        evt.Skip(); // Event didn't catch
        break;
    }
    return;
}
