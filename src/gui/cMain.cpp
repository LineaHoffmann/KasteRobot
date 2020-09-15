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

cMain::cMain() : wxFrame (nullptr, wxID_ANY, "Robot Control Interface", wxPoint(30,30), wxSize(1280,1024))
{
    // Icon for the window
    wxIcon icon(wxT("../examples/icon.png"), wxBITMAP_TYPE_PNG);
    this->SetIcon(icon);

    // Main sizer
    wxBoxSizer *sizerMain = new wxBoxSizer(wxHORIZONTAL);
    // Right sizer
    wxBoxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    // Left sizer
    wxBoxSizer *sizerLeft = new wxBoxSizer(wxHORIZONTAL);

    // Main splitter window
    wxSplitterWindow *splitterMain = new wxSplitterWindow(this, wxID_ANY);
    splitterMain->SetSashGravity(0.5);
    splitterMain->SetMinimumPaneSize(20);
    sizerMain->Add(splitterMain, 1, wxEXPAND, 0);

    // Left side panel for numeric information and the like
    mLeftPanel = new wxPanel(splitterMain, wxID_ANY);
    //Right side panel for two wxPanel image feeds (cImagePanel)
    mRightSplitpanel = new wxSplitterWindow(splitterMain, wxID_ANY);

    // Creation of GUI elements for left side
    mBtn1 = new wxButton(mLeftPanel, 20001, "Button 1", wxPoint(10,10), wxSize(100,30));
    mBtn2 = new wxButton(mLeftPanel, 20002, "Button 2", wxPoint(10,50), wxSize(100,30));
    mText1 = new wxTextCtrl(mLeftPanel, wxID_ANY, "", wxPoint(10,80), wxSize(150,50));
    mList1 = new wxListBox(mLeftPanel, wxID_ANY, wxPoint(10,130), wxSize(150,400));

    // Adding left controls to left sizer
    sizerLeft->Add(mBtn1, 1, wxEXPAND, 0);
    sizerLeft->Add(mBtn2, 1, wxEXPAND, 0);
    sizerLeft->Add(mText1, 1, wxEXPAND, 0);
    sizerLeft->Add(mList1, 1, wxEXPAND, 0);

    // Background color for leftside panel
    mLeftPanel->SetBackgroundColour(wxColor(*wxLIGHT_GREY));

    // Assign left sizer to left panel
    mLeftPanel->SetSizer(sizerLeft);

    // Camera viewing area
    mCameraPanel = new cImagePanel(mRightSplitpanel, wxID_ANY, wxT("../examples/Astronaut1.jpg"), wxBITMAP_TYPE_ANY);
    sizerRight->Add(mCameraPanel, 1, wxEXPAND, 0);
    mRobotPanel = new cImagePanel(mRightSplitpanel, wxID_ANY, wxT("../examples/Astronaut1.jpg"), wxBITMAP_TYPE_JPEG);
    sizerRight->Add(mRobotPanel, 1, wxEXPAND, 0);

    mRightSplitpanel->SetSashGravity(0.5);
    mRightSplitpanel->SetMinimumPaneSize(20);

    //Assign right sizer to right panel
    sizerRight->Add(mRightSplitpanel);

    // Splitting into right/left sides
    splitterMain->SplitVertically(mLeftPanel, mRightSplitpanel);
    this->SetSizer(sizerMain);
    sizerMain->SetSizeHints(this);

    // Splitting right side into top/bottom
    mRightSplitpanel->SplitHorizontally(mCameraPanel, mRobotPanel);
    sizerRight->SetSizeHints(splitterMain);

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

cMain::~cMain()
{
}

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
