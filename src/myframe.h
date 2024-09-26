#ifndef MYFRAME_H
#define MYFRAME_H

#include <wx/wx.h>
#include "game.h"

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

private:
    void OnRadioButtonSelected(wxCommandEvent& event);
    void OnStartButtonClicked(wxCommandEvent& event);

    wxTextCtrl* playerNameTextCtrl;
    wxStaticText* playerNameLabel;
    wxRadioButton* humanRadioButton;
    wxRadioButton* botRadioButton;
    wxButton* startButton;

    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_HumanRadioButton = PlayerType::_HUMAN,
    ID_BotRadioButton = PlayerType::_BOT,
    ID_StartButton = PlayerType::_START_BUTTON
};

#endif // MYFRAME_H
