#include "cmain.h"

// Event -> Function binding table
// Event binding name enum is in the header
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
    // Icon for the window
    wxIcon icon(wxT("../resources/icon.png"), wxBITMAP_TYPE_PNG);
    SetIcon(icon);

    // Main grid sizer
    SetSizeHints( wxDefaultSize, wxDefaultSize );
    wxGridSizer* gridSizer;
    gridSizer = new wxGridSizer( 0, 2, 0, 0 );

    // Image viewing panel 1
    mPanelView1 = new cImagePanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    // Text control area
    mTextCtrl = new wxTextCtrl(this,
                               wxID_ANY,
                               wxEmptyString,
                               wxDefaultPosition,
                               wxDefaultSize,
                               0|wxVSCROLL|wxBORDER|wxTE_READONLY | wxTE_MULTILINE | wxTE_LEFT | wxTE_CHARWRAP);
    mTextLog = new wxLogTextCtrl(mTextCtrl); // For writing to gui log system-wide, should be thread safe
    wxLog::SetActiveTarget(mTextLog);
    // Image viewing panel 2
    mPanelView2 = new cImagePanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    // Notebook area
    mNotebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER);
    // Order dictates placement, top left - top right - bottom left - bottom right
    gridSizer->Add( mTextCtrl, 1, wxEXPAND | wxALL, 0 );
    gridSizer->Add( mPanelView1, 1, wxEXPAND | wxALL, 0 );
    gridSizer->Add( mNotebook, 1, wxEXPAND | wxALL, 0 );
    gridSizer->Add( mPanelView2, 1, wxEXPAND | wxALL, 0 );

    // Notebook contents
    wxBoxSizer *mSizerNotebookGeneral = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *mSizerNotebookRobot = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *mSizerNotebookGripper = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *mSizerNotebookCamera = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *mSizerNotebookDatabase = new wxBoxSizer(wxHORIZONTAL);
    wxPanel *mNotebookGeneral = new wxPanel(mNotebook,
                                            wxID_ANY,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxTAB_TRAVERSAL | wxNO_BORDER,
                                            "subPanel1");
    wxPanel *mNotebookRobot = new wxPanel(mNotebook,
                                          wxID_ANY,
                                          wxDefaultPosition,
                                          wxDefaultSize,
                                          wxTAB_TRAVERSAL | wxNO_BORDER,
                                          "subPanel2");
    wxPanel *mNotebookGripper = new wxPanel(mNotebook,
                                            wxID_ANY,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxTAB_TRAVERSAL | wxNO_BORDER,
                                            "subPanel3");
    wxPanel *mNotebookCamera = new wxPanel(mNotebook,
                                           wxID_ANY,
                                           wxDefaultPosition,
                                           wxDefaultSize,
                                           wxTAB_TRAVERSAL | wxNO_BORDER,
                                           "subPanel4");
    wxPanel *mNotebookDatabase = new wxPanel(mNotebook,
                                             wxID_ANY,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxTAB_TRAVERSAL | wxNO_BORDER,
                                             "subPanel5");
    mNotebookGeneral->SetSizer(mSizerNotebookGeneral);
    mNotebookRobot->SetSizer(mSizerNotebookRobot);
    mNotebookGripper->SetSizer(mSizerNotebookGripper);
    mNotebookCamera->SetSizer(mSizerNotebookCamera);
    mNotebookDatabase->SetSizer(mSizerNotebookDatabase);
    mNotebook->InsertPage(0,mNotebookGeneral, "General");
    mNotebook->InsertPage(1,mNotebookRobot, "Robot");
    mNotebook->InsertPage(2,mNotebookGripper, "Gripper");
    mNotebook->InsertPage(3,mNotebookCamera, "Camera");
    mNotebook->InsertPage(4,mNotebookDatabase, "Database");
    wxColour LIGHT_BLUE = wxColor(84,88,94);
    mNotebook->SetBackgroundColour(LIGHT_BLUE);
    mNotebookGeneral->SetBackgroundColour(LIGHT_BLUE);
    mNotebookRobot->SetBackgroundColour(LIGHT_BLUE);
    mNotebookGripper->SetBackgroundColour(LIGHT_BLUE);
    mNotebookCamera->SetBackgroundColour(LIGHT_BLUE);
    mNotebookDatabase->SetBackgroundColour(LIGHT_BLUE);
    // Tree list creation (shows in mNotebookGeneral tab)
    mTreeList = new wxTreeListCtrl(mNotebookGeneral,
                                             wxID_ANY,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxTL_DEFAULT_STYLE);
    mTreeList->AppendColumn("Description",
                                      wxCOL_WIDTH_DEFAULT,
                                      wxALIGN_LEFT,
                                      wxCOL_RESIZABLE | wxCOL_SORTABLE);
    mTreeList->AppendColumn("Data",
                                      wxCOL_WIDTH_DEFAULT,
                                      wxALIGN_RIGHT,
                                      wxCOL_RESIZABLE);
    mSizerNotebookGeneral->Add(mTreeList, wxSizerFlags(1).Expand());

    // NOTE: This is where to add items to the tree
    wxTreeListItem root = mTreeList->GetRootItem();
    // Top level
    mTreeRootRobot = new wxTreeListItem(mTreeList->AppendItem(root, "Robot"));
    mTreeRootCamera = new wxTreeListItem(mTreeList->AppendItem(root, "Camera"));
    mTreeRootGripper = new wxTreeListItem(mTreeList->AppendItem(root, "Gripper"));
    mTreeRootDatabase = new wxTreeListItem(mTreeList->AppendItem(root, "Database"));
    // Sub levels
    mTreeRobotState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "State"));
    mTreeRobotIP = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "IP"));
    mTreeRobotPort = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootRobot, "Port"));
    mTreeCameraState = new wxTreeListItem(mTreeList->AppendItem(*mTreeRootCamera, "State"));

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
    if (evt.GetId() == ID_MENU_SAVE_LOG)

    // Updating event for refreshing the info tree
    // TODO: Write this
    // Collect info from all available object through
    // the logic controller, in a thread safe manner
    evt.Skip();
}
void cMain::OnButtonPress(wxCommandEvent &evt) {
    // Switchcase to the appropriate response
    switch (evt.GetId()) {
    case ID_MENU_SAVE_LOG:
        logstd("Menu->Save Log clicked");
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
        evt.Skip();
        break;
    case ID_BTN_ROBOT_DISCONNECT:
        logstd("Robot->Disconnect clicked");
        evt.Skip();
        break;
    case ID_BTN_GRIPPER_CONNECT:
        logstd("Gripper->Connect clicked");
        evt.Skip();
        break;
    case ID_BTN_GRIPPER_DISCONNECT:
        logstd("Gripper->Disconnect clicked");
        evt.Skip();
        break;
    case ID_BTN_CAMERA_CONNECT:
        logstd("Camera->Connect clicked");
        evt.Skip();
        break;
    case ID_BTN_CAMERA_DISCONNECT:
        logstd("Camera->Disconnect clicked");
        evt.Skip();
        break;
    case ID_BTN_DATABASE_CONNECT:
        logstd("Database->Connect clicked");
        evt.Skip();
        break;
    case ID_BTN_DATABASE_DISCONNECT:
        logstd("Database->Disconnect clicked");
        evt.Skip();
        break;
    default:
        break;
    }
    return;
}
