#include "cMain.h"

#include <iostream>

// Event - Function binding table
// IDs should be defined as more useful names some other place
wxBEGIN_EVENT_TABLE(cMain, wxFrame)
    EVT_MENU(10001, cMain::OnMenuSaveLog)
    EVT_MENU(10002, cMain::OnMenuSaveSnap)
    EVT_MENU(10003, cMain::OnMenuExit)
    EVT_MENU(10004, cMain::OnMenuAbout)

    EVT_BUTTON(20001, cMain::OnBtn1Clicked)
    EVT_BUTTON(20002, cMain::OnBtn2Clicked)
wxEND_EVENT_TABLE()

/**
 * @brief Builds the GUI as a wxFrame, to run in a parent wxWindow (see cApp.*)
 */
cMain::cMain() : wxFrame (nullptr, wxID_ANY, "Robot Control Interface", wxPoint(30,30), wxSize(1280,1024))
{
    // Icon for the window
    wxIcon icon(wxT("../examples/icon.png"), wxBITMAP_TYPE_PNG);
    this->SetIcon(icon);

    // Init of sizers, window (+left/right side contents), and top menu
    this->initSizers();
    this->initMainWindow();
    this->initMenu();
}

cMain::~cMain()
{
}
// This will be handler functions galore
void cMain::OnBtn1Clicked(wxCommandEvent &evt)
{
    std::cout << "Button 1 Clicked" << std::endl;

    mList1->AppendString(mText1->GetValue());
    mText1->Clear();

    evt.Skip();
}
void cMain::OnBtn2Clicked(wxCommandEvent &evt)
{
    std::cout << "Button 2 Clicked" << std::endl;
    evt.Skip();
}
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
    Close();
    evt.Skip();
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
    mSizerLeftGeneral = new wxBoxSizer(wxHORIZONTAL);
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
    mCameraPanel = new cImagePanel(mRightSplitpanel, wxID_ANY, wxT("../examples/Astronaut1.jpg"), wxBITMAP_TYPE_ANY);
    mSizerRight->Add(mCameraPanel, 1, wxEXPAND, 0);
    mRobotPanel = new cImagePanel(mRightSplitpanel, wxID_ANY, wxT("../examples/Astronaut1.jpg"), wxBITMAP_TYPE_JPEG);
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
    mBtn1 = new wxButton(mLeftSubPanelGeneral, 20001, "Button 1", wxPoint(10,10), wxSize(100,30));
    mBtn2 = new wxButton(mLeftSubPanelGeneral, 20002, "Button 2", wxPoint(10,50), wxSize(100,30));
    mText1 = new wxTextCtrl(mLeftSubPanelGeneral, wxID_ANY, "", wxPoint(10,80), wxSize(150,50));
    mList1 = new wxListBox(mLeftSubPanelGeneral, wxID_ANY, wxPoint(10,130), wxSize(150,400));

    // Adding objects to the tab sizer
    mSizerLeftGeneral->Add(mBtn1, 1, wxEXPAND, 0);
    mSizerLeftGeneral->Add(mBtn2, 1, wxEXPAND, 0);
    mSizerLeftGeneral->Add(mText1, 1, wxEXPAND, 0);
    mSizerLeftGeneral->Add(mList1, 1, wxEXPAND, 0);
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
