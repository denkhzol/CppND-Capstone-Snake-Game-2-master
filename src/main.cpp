#include <iostream>
#include <string>
#include "controller.h"
#include "game.h"
#include "renderer.h"
#include <wx/wx.h>
#include "myframe.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame("Player Selection");
    frame->Show(true);
    return true;
}