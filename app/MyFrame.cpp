#include "MyFrame.h"
#include "wx/gtk/frame.h"
#include "wx/gtk/menu.h"
#include "wx/gtk/menuitem.h"
#include "wx/log.h"
#include "wx/msgdlg.h"

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Hello, NES!")
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to NESpp!");

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from NESpp!");
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("Example string", "About NESpp", wxOK | wxICON_INFORMATION);
}