#include "myApp.h"
#include "MyFrame.h"

wxIMPLEMENT_APP(myApp);

bool myApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}