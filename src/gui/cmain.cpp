#include "cmain.h"

// Event -> Function binding table
// Event binding name enum is in the header
wxBEGIN_EVENT_TABLE(cMain, wxFrame)
    // Timer bindings
    EVT_TIMER(ID_TIMER_CAMERA_UPDATE, cMain::OnTimerCameraUpdate)
    // Menu bindings
    EVT_MENU(ID_MENU_SAVE_LOG, cMain::OnMenuSaveLog)
    EVT_MENU(ID_MENU_SAVE_SNAPSHOT, cMain::OnMenuSaveSnap)
    EVT_MENU(ID_MENU_EXIT, cMain::OnMenuExit)
    EVT_MENU(ID_MENU_ABOUT, cMain::OnMenuAbout)
    // Connect / disconnect button bindings
    EVT_BUTTON(ID_BTN_ROBOT_CONNECT, cMain::OnBtnRobotConnect)
    EVT_BUTTON(ID_BTN_ROBOT_DISCONNECT, cMain::OnBtnRobotDisconnect)
    EVT_BUTTON(ID_BTN_GRIPPER_CONNECT, cMain::OnBtnGripperConnect)
    EVT_BUTTON(ID_BTN_GRIPPER_DISCONNECT, cMain::OnBtnGripperDisconnect)
    EVT_BUTTON(ID_BTN_CAMERA_CONNECT, cMain::OnBtnCameraConnect)
    EVT_BUTTON(ID_BTN_CAMERA_DISCONNECT, cMain::OnBtnCameraDisconnect)
    EVT_BUTTON(ID_BTN_DATABASE_CONNECT, cMain::OnBtnDatabaseConnect)
    EVT_BUTTON(ID_BTN_DATABASE_DISCONNECT, cMain::OnBtnDatabaseDisconnect)
wxEND_EVENT_TABLE()

/**
 * @brief Builds the GUI as a wxFrame to be run by a parent wxApp (see capp.*)
 */
cMain::cMain() : wxFrame (nullptr, wxID_ANY, "Robot Control Interface", wxPoint(30,30), wxSize(1280,1024)),
                 mTimerCamera(this, ID_TIMER_CAMERA_UPDATE)
{
    // Icon for the window
    wxIcon icon(wxT("../resources/icon.png"), wxBITMAP_TYPE_PNG);

    // Init of the layer linker
    mLinker = new cLinker();

    // Init of sizers, window (+left/right side contents), and top menu
    initSizers();
    initMainWindow();
    initMenu();

    // Starting the timer for the view update
    // Loads cv::Mat from camera, converts to wxImage, and updates view
    mTimerCamera.Start(50);
}
cMain::~cMain()
{
}
void cMain::addLinker(cLinker* linker) {
    mLinker = linker;
}
// This will be handler functions galore
void cMain::OnTimerCameraUpdate(wxTimerEvent &evt) {
    // NOTE: Maybe cLinker should get a bool cLinker::isOK() to check pointers in
    if (mLinker == nullptr) return;
    cv::Mat input = mLinker->getCameraFrame();
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
    mCameraPanel->setNewImage(output);
    evt.Skip();
}
void cMain::OnMenuSaveLog(wxCommandEvent &evt) {
    logstd("Menu->Save Log clicked");
    evt.Skip();
}
void cMain::OnMenuSaveSnap(wxCommandEvent &evt) {
    logstd("Menu->Save Snapshot clicked");
    evt.Skip();
}
void cMain::OnMenuExit(wxCommandEvent &evt) {
    // Sending to cout because the application will die now
    std::cout << "Menu->Exit clicked" << std::endl;
    Close(true);
    evt.Skip();
}
void cMain::OnMenuAbout(wxCommandEvent &evt) {
    logstd("Menu->About clicked");
    wxString msg;
    msg.Append("3rd Semester Project in Robotics\n"
               "University of Southern Denmark\n\n"
               "Created by\n"
               "Jacob Warrer\n"
               "Jonas Lorentzen\n"
               "Linea Hoffmann\n"
               "Mathias Halbro\n"
               "Mikkel Joergensen\n"
               "Soeren Pedersen");
    wxMessageBox(msg, "About");
    evt.Skip();
}
void cMain::OnBtnRobotConnect(wxCommandEvent &evt) {
    logstd("Robot->Connect clicked");
    evt.Skip();
}
void cMain::OnBtnRobotDisconnect(wxCommandEvent &evt) {
    logstd("Robot->Disconnect clicked");
    evt.Skip();
}
void cMain::OnBtnGripperConnect(wxCommandEvent &evt) {
    logstd("Gripper->Connect clicked");
    evt.Skip();
}
void cMain::OnBtnGripperDisconnect(wxCommandEvent &evt) {
    logstd("Gripper->Disconnect clicked");
    evt.Skip();
}
void cMain::OnBtnCameraConnect(wxCommandEvent &evt) {
    logstd("Camera->Connect clicked");
    evt.Skip();
}
void cMain::OnBtnCameraDisconnect(wxCommandEvent &evt) {
    logstd("Camera->Disconnect clicked");
    evt.Skip();
}
void cMain::OnBtnDatabaseConnect(wxCommandEvent &evt) {
    logstd("Database->Connect clicked");
    evt.Skip();
}
void cMain::OnBtnDatabaseDisconnect(wxCommandEvent &evt) {
    logstd("Database->Disconnect clicked");
    evt.Skip();
}
/**
 * @brief Initializes all the sizers required
 */
void cMain::initSizers()
{
    // Main sizer
    mSizerMain = new wxBoxSizer(wxHORIZONTAL);
    // Right sizer
    mSizerRight = new wxBoxSizer(wxVERTICAL);
    // Left sizer
    mSizerLeft = new wxBoxSizer(wxHORIZONTAL);
    // Left sub sizers
    mSizerLeftGeneral = new wxBoxSizer(wxVERTICAL);
    mSizerLeftRobot = new wxBoxSizer(wxHORIZONTAL);
    mSizerLeftGripper = new wxBoxSizer(wxHORIZONTAL);
    mSizerLeftCamera = new wxBoxSizer(wxHORIZONTAL);
    mSizerLeftDatabase = new wxBoxSizer(wxHORIZONTAL);
}
/**
 * @brief Initializes the split main window
 */
void cMain::initMainWindow()
{
    // Main split window
    mSplitterMain = new wxSplitterWindow(this, wxID_ANY);
    mSplitterMain->SetSashGravity(0.5);
    mSplitterMain->SetMinimumPaneSize(20);
    mSizerMain->Add(mSplitterMain, 1, wxEXPAND, 0);

    this->initLeftPanel();
    this->initRightPanel();

    // Splitting into right/left sides (left and right panel inits has to be before this)
    mSplitterMain->SplitVertically(mLeftBookPanel, mRightSplitpanel);
    this->SetSizer(mSizerMain);
    mSizerMain->SetSizeHints(this);
}
/**
 * @brief Initializes the left side of the split main window
 */
void cMain::initLeftPanel()
{
    // Left side panel for numeric information and the like
    mLeftBookPanel = new wxNotebook(mSplitterMain, wxID_ANY);

    // Sub-panels for left side
    mLeftSubPanelGeneral = new wxPanel(mLeftBookPanel, wxID_ANY);
    mLeftSubPanelRobot = new wxPanel(mLeftBookPanel, wxID_ANY);
    mLeftSubPanelGripper = new wxPanel(mLeftBookPanel, wxID_ANY);
    mLeftSubPanelCamera = new wxPanel(mLeftBookPanel, wxID_ANY);
    mLeftSubPanelDatabase = new wxPanel(mLeftBookPanel, wxID_ANY);

    // Inserting sub pages/panels into left side book
    mLeftBookPanel->InsertPage(0,mLeftSubPanelGeneral, "General");
    mLeftBookPanel->InsertPage(1,mLeftSubPanelRobot, "Robot");
    mLeftBookPanel->InsertPage(2,mLeftSubPanelGripper, "Gripper");
    mLeftBookPanel->InsertPage(3,mLeftSubPanelCamera, "Camera");
    mLeftBookPanel->InsertPage(4,mLeftSubPanelDatabase, "Database");

    // Background colorings
    wxColour LIGHT_BLUE = wxColor(84,88,94);
    mLeftBookPanel->SetBackgroundColour(LIGHT_BLUE);
    mLeftSubPanelGeneral->SetBackgroundColour(LIGHT_BLUE);
    mLeftSubPanelRobot->SetBackgroundColour(LIGHT_BLUE);
    mLeftSubPanelGripper->SetBackgroundColour(LIGHT_BLUE);
    mLeftSubPanelCamera->SetBackgroundColour(LIGHT_BLUE);
    mLeftSubPanelDatabase->SetBackgroundColour(LIGHT_BLUE);

    // Creation of GUI elements for left side tabs
    initTabGeneral();
    initTabRobot();
    initTabGripper();
    initTabCamera();
    initTabDatabase();

    // Assign left sizer to left notebook panel
    mLeftBookPanel->SetSizer(mSizerLeft);

    // Assigning sub-sizers for left tabs
    mLeftSubPanelGeneral->SetSizer(mSizerLeftGeneral);
    mLeftSubPanelRobot->SetSizer(mSizerLeftRobot);
    mLeftSubPanelGripper->SetSizer(mSizerLeftGripper);
    mLeftSubPanelCamera->SetSizer(mSizerLeftCamera);
    mLeftSubPanelDatabase->SetSizer(mSizerLeftDatabase);
}
/**
 * @brief Initializes the right side of the split main window
 */
void cMain::initRightPanel()
{
    //Right side panel for two wxPanel image feeds (cImagePanel)
    mRightSplitpanel = new wxSplitterWindow(mSplitterMain, wxID_ANY,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxSP_3D, "Splitter"
                                            );

    // Camera viewing area
    mCameraPanel = new cImagePanel(mRightSplitpanel, wxID_ANY);
    mSizerRight->Add(mCameraPanel, 1, wxEXPAND, 0);
    mRobotPanel = new cImagePanel(mRightSplitpanel, wxID_ANY);
    mSizerRight->Add(mRobotPanel, 1, wxEXPAND, 0);

    //Assign right sizer to right panel
    mSizerRight->Add(mRightSplitpanel);

    // Splitting right side into top/bottom
    mRightSplitpanel->SplitHorizontally(mCameraPanel, mRobotPanel);
    mRightSplitpanel->SetSashGravity(0.5);
    mSizerRight->SetSizeHints(mSplitterMain);
}
void cMain::initMenu()
{
    // Menu bar creation
    mMenuBar = new wxMenuBar();
    this->SetMenuBar(mMenuBar);
    wxMenu *menuFile = new wxMenu();
    menuFile->Append(ID_MENU_SAVE_LOG, "Save Log");
    menuFile->Append(ID_MENU_SAVE_SNAPSHOT, "Save Snapshot");
    menuFile->Append(ID_MENU_EXIT, "Exit");
    menuFile->Append(ID_MENU_ABOUT, "About");
    mMenuBar->Append(menuFile,"File");
}
void cMain::initTabGeneral()
{
    // Creation of GUI objects
    // The General tab is information only, displayed in a treelist
    mTabGeneralTreeList = new wxTreeListCtrl(mLeftSubPanelGeneral,
                                             wxID_ANY,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxTL_DEFAULT_STYLE);
    mTabGeneralTreeList->AppendColumn("Description",
                                      100, // Would use wxCOL_WIDTH_AUTOSIZE, but it negates resizeable??
                                      wxALIGN_LEFT,
                                      wxCOL_RESIZABLE | wxCOL_SORTABLE);
    mTabGeneralTreeList->AppendColumn("Data",
                                      wxCOL_WIDTH_AUTOSIZE,
                                      wxALIGN_RIGHT,
                                      wxCOL_RESIZABLE);

    wxTreeListItem root = mTabGeneralTreeList->GetRootItem();

    // Allocating all of the various data fields / entries (could be prettier..)
    mTabGeneralSubRobot = new wxTreeListItem();
    mTabGeneralSubRobotTCP = new wxTreeListItem();
    mTabGeneralSubRobotJoints = new wxTreeListItem();
    mTabGeneralSubRobotIP = new wxTreeListItem();
    mTabGeneralSubRobotState = new wxTreeListItem();
    mTabGeneralSubCamera = new wxTreeListItem();
    mTabGeneralSubCameraIP = new wxTreeListItem();
    mTabGeneralSubCameraState = new wxTreeListItem();
    mTabGeneralSubGripper = new wxTreeListItem();
    mTabGeneralSubGripperIP = new wxTreeListItem();
    mTabGeneralSubGripperState = new wxTreeListItem();
    mTabGeneralSubDatabase = new wxTreeListItem();
    mTabGeneralSubDatabaseHost = new wxTreeListItem();
    mTabGeneralSubDatabaseUser = new wxTreeListItem();
    mTabGeneralSubDatabaseState = new wxTreeListItem();
    mTabGeneralSubDatabaseLastEntry = new wxTreeListItem();

    // Building top level structure
    *mTabGeneralSubRobot = mTabGeneralTreeList->AppendItem(root, "Robot");
    *mTabGeneralSubCamera = mTabGeneralTreeList->AppendItem(root, "Camera");
    *mTabGeneralSubGripper = mTabGeneralTreeList->AppendItem(root, "Gripper");
    *mTabGeneralSubDatabase = mTabGeneralTreeList->AppendItem(root, "Database");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubRobot, 1, "NotConnected");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubCamera, 1, "NotConnected");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubGripper, 1, "NotConnected");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubDatabase, 1, "NotConnected");

    // Adding robot substructure
    *mTabGeneralSubRobotTCP = mTabGeneralTreeList->AppendItem(*mTabGeneralSubRobot, "TCP");
    *mTabGeneralSubRobotJoints = mTabGeneralTreeList->AppendItem(*mTabGeneralSubRobot, "Joints");
    *mTabGeneralSubRobotIP = mTabGeneralTreeList->AppendItem(*mTabGeneralSubRobot, "IP");
    *mTabGeneralSubRobotState = mTabGeneralTreeList->AppendItem(*mTabGeneralSubRobot, "State");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubRobotTCP, 1, "x:---  y:---  z:---  rx:--- ry:--- rz:---");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubRobotJoints, 1, "1:---  2:---  3:---  4:---  5:---  6:---");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubRobotIP, 1, "---.---.-.-");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubRobotState, 1, "NO CONNECTION");

    // Adding camera substructure
    *mTabGeneralSubCameraIP = mTabGeneralTreeList->AppendItem(*mTabGeneralSubCamera, "IP");
    *mTabGeneralSubCameraState = mTabGeneralTreeList->AppendItem(*mTabGeneralSubCamera, "State");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubCameraIP, 1, "---.---.-.-");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubCameraState, 1, "NO CONNECTION");

    // Adding gripper substructure
    *mTabGeneralSubGripperIP = mTabGeneralTreeList->AppendItem(*mTabGeneralSubGripper, "IP");
    *mTabGeneralSubGripperState = mTabGeneralTreeList->AppendItem(*mTabGeneralSubGripper, "State");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubGripperIP, 1, "---.---.-.-");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubGripperState, 1, "NO CONNECTION");

    // Adding database substructure
    *mTabGeneralSubDatabaseHost = mTabGeneralTreeList->AppendItem(*mTabGeneralSubDatabase, "Host");
    *mTabGeneralSubDatabaseUser = mTabGeneralTreeList->AppendItem(*mTabGeneralSubDatabase, "User");
    *mTabGeneralSubDatabaseState = mTabGeneralTreeList->AppendItem(*mTabGeneralSubDatabase, "State");
    *mTabGeneralSubDatabaseLastEntry = mTabGeneralTreeList->AppendItem(*mTabGeneralSubDatabase, "Last Entry");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubDatabaseHost, 1, "---.---.-.-");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubDatabaseUser, 1, "----");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubDatabaseState, 1, "NO CONNECTION");
    mTabGeneralTreeList->SetItemText(*mTabGeneralSubDatabaseLastEntry, 1, "NO CONNECTION");

    // Allocation of text area for log output
    mTabGeneralTextCtrl = new wxTextCtrl(mLeftSubPanelGeneral,
                                         wxID_ANY,
                                         "",
                                         wxDefaultPosition,
                                         wxDefaultSize,
                                         wxTE_READONLY | wxTE_MULTILINE | wxTE_LEFT | wxTE_CHARWRAP);
    mTabGeneralLog = new wxLogTextCtrl(mTabGeneralTextCtrl);
    wxLog::SetActiveTarget(mTabGeneralLog);

    // Adding objects to the tab sizer
    mSizerLeftGeneral->Add(mTabGeneralTextCtrl, wxSizerFlags(1).Expand());
    mSizerLeftGeneral->Add(mTabGeneralTreeList, wxSizerFlags(1).Expand());
}

void cMain::initTabRobot()
{
    // Creation of GUI objects
    mTabRobotConnectBtn = new wxButton(mLeftSubPanelRobot, ID_BTN_ROBOT_CONNECT, "Connect");
    mTabRobotDisconnectBtn = new wxButton(mLeftSubPanelRobot, ID_BTN_ROBOT_DISCONNECT, "Disonnect");
    mTabRobotIpEntryTxtCtrl = new wxTextCtrl(mLeftSubPanelRobot, wxID_ANY,
                                             "IP address",
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxTE_LEFT);

    // Adding objects to the tab sizer
    mSizerLeftRobot->Add(mTabRobotConnectBtn);
    mSizerLeftRobot->Add(mTabRobotDisconnectBtn);
    mSizerLeftRobot->Add(mTabRobotIpEntryTxtCtrl);

}
void cMain::initTabGripper()
{
    // Creation of GUI objects
    mTabGripperConnectBtn = new wxButton(mLeftSubPanelGripper, ID_BTN_GRIPPER_CONNECT, "Connect");
    mTabGripperDisconnectBtn = new wxButton(mLeftSubPanelGripper, ID_BTN_GRIPPER_DISCONNECT, "Disconnect");
    mTabGripperIpEntryTxtCtrl = new wxTextCtrl(mLeftSubPanelGripper, wxID_ANY,
                                               "IP address",
                                               wxDefaultPosition,
                                               wxDefaultSize,
                                               wxTE_LEFT);

    // Adding objects to the tab sizer
    mSizerLeftGripper->Add(mTabGripperConnectBtn);
    mSizerLeftGripper->Add(mTabGripperDisconnectBtn);
    mSizerLeftGripper->Add(mTabGripperIpEntryTxtCtrl);

}
void cMain::initTabCamera()
{
    // Creation of GUI objects
    mTabCameraConnectBtn = new wxButton(mLeftSubPanelCamera, ID_BTN_CAMERA_CONNECT, "Connect");
    mTabCameraDisconnectBtn = new wxButton(mLeftSubPanelCamera, ID_BTN_CAMERA_DISCONNECT, "Disconnect");
    mTabCameraIpEntryTxtCtrl = new wxTextCtrl(mLeftSubPanelCamera, wxID_ANY,
                                              "IP address",
                                              wxDefaultPosition,
                                              wxDefaultSize,
                                              wxTE_LEFT);

    // Adding objects to the tab sizer
    mSizerLeftCamera->Add(mTabCameraConnectBtn);
    mSizerLeftCamera->Add(mTabCameraDisconnectBtn);
    mSizerLeftCamera->Add(mTabCameraIpEntryTxtCtrl);
}
void cMain::initTabDatabase()
{
    // Creation of GUI objects
    mTabDatabaseConnectBtn = new wxButton(mLeftSubPanelDatabase, ID_BTN_DATABASE_CONNECT, "Connect");
    mTabDatabaseDisconnectBtn = new wxButton(mLeftSubPanelDatabase, ID_BTN_DATABASE_DISCONNECT, "Disconnect");
    mTabDatabaseHostEntryTxtCtrl = new wxTextCtrl(mLeftSubPanelDatabase, wxID_ANY,
                                                  "Host",
                                                  wxDefaultPosition,
                                                  wxDefaultSize,
                                                  wxTE_LEFT);
    mTabDatabaseUserEntryTxtCtrl = new wxTextCtrl(mLeftSubPanelDatabase, wxID_ANY,
                                                  "User",
                                                  wxDefaultPosition,
                                                  wxDefaultSize,
                                                  wxTE_LEFT);
    mTabDatabasePasswordEntryTxtCtrl = new wxTextCtrl(mLeftSubPanelDatabase, wxID_ANY,
                                                      "Password",
                                                      wxDefaultPosition,
                                                      wxDefaultSize,
                                                      wxTE_LEFT);

    // Adding objects to the tab sizer
    mSizerLeftDatabase->Add(mTabDatabaseConnectBtn);
    mSizerLeftDatabase->Add(mTabDatabaseDisconnectBtn);
    mSizerLeftDatabase->Add(mTabDatabaseHostEntryTxtCtrl);
    mSizerLeftDatabase->Add(mTabDatabaseUserEntryTxtCtrl);
    mSizerLeftDatabase->Add(mTabDatabasePasswordEntryTxtCtrl);
}
