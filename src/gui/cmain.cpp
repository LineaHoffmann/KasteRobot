#include "cmain.h"

#include <iostream>

// Event -> Function binding table
// IDs should be defined as more useful names some other place
wxBEGIN_EVENT_TABLE(cMain, wxFrame)
    EVT_MENU(10001, cMain::OnMenuSaveLog)
    EVT_MENU(10002, cMain::OnMenuSaveSnap)
    EVT_MENU(10003, cMain::OnMenuExit)
    EVT_MENU(10004, cMain::OnMenuAbout)
wxEND_EVENT_TABLE()

/**
 * @brief Builds the GUI as a wxFrame to be run by a parent wxApp (see capp.*)
 */
cMain::cMain() : wxFrame (nullptr, wxID_ANY, "Robot Control Interface", wxPoint(30,30), wxSize(1280,1024))
{
    // Icon for the window
    wxIcon icon(wxT("../src/gui/icon.png"), wxBITMAP_TYPE_PNG);
    SetIcon(icon);

    // Init of the layer linker
    mLinker = new cLinker();

    // Init of sizers, window (+left/right side contents), and top menu
    initSizers();
    initMainWindow();
    initMenu();
}

cMain::~cMain()
{
}
// This will be handler functions galore
void cMain::OnMenuSaveLog(wxCommandEvent &evt) {
    std::cout << "Menu->Save Log clicked" << std::endl;
    evt.Skip();
}
void cMain::OnMenuSaveSnap(wxCommandEvent &evt) {
    std::cout << "Menu->Save Snapshot clicked" << std::endl;
    evt.Skip();
}
void cMain::OnMenuExit(wxCommandEvent &evt) {
    std::cout << "Menu->Exit clicked" << std::endl;
    Close(true);
}
void cMain::OnMenuAbout(wxCommandEvent &evt) {
    std::cout << "Menu->About clicked" << std::endl;
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
    return;
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
    mRightSplitpanel = new wxSplitterWindow(mSplitterMain, wxID_ANY);

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
    menuFile->Append(10001, "Save Log");
    menuFile->Append(10002, "Save Snapshot");
    menuFile->Append(10003, "Exit");
    menuFile->Append(10004, "About");
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

    // Allocation of text area for cout & cerr output
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
    // Adding objects to the tab sizer
}
void cMain::initTabGripper()
{
    // Creation of GUI objects
    // Adding objects to the tab sizer
}
void cMain::initTabCamera()
{
    // Creation of GUI objects
    // Adding objects to the tab sizer
}
void cMain::initTabDatabase()
{
    // Creation of GUI objects
    // Adding objects to the tab sizer
}
