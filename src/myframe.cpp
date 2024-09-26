#include <iostream>
#include <string>
#include "controller.h"
#include "game.h"
#include "renderer.h"
#include <wx/wx.h>

#include "myframe.h"

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_RADIOBUTTON(ID_HumanRadioButton, MyFrame::OnRadioButtonSelected)
    EVT_RADIOBUTTON(ID_BotRadioButton, MyFrame::OnRadioButtonSelected)
    EVT_BUTTON(ID_StartButton, MyFrame::OnStartButtonClicked)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(300, 250))
{
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    playerNameLabel = new wxStaticText(panel, wxID_ANY, wxT("Player Name:"));
    vbox->Add(playerNameLabel, 0, wxLEFT | wxTOP, 10);

    playerNameTextCtrl = new wxTextCtrl(panel, wxID_ANY);
    vbox->Add(playerNameTextCtrl, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);

    humanRadioButton = new wxRadioButton(panel, ID_HumanRadioButton, wxT("Human"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    vbox->Add(humanRadioButton, 0, wxLEFT | wxTOP, 10);

    botRadioButton = new wxRadioButton(panel, ID_BotRadioButton, wxT("Bot"));
    vbox->Add(botRadioButton, 0, wxLEFT | wxTOP, 10);

    startButton = new wxButton(panel, ID_StartButton, wxT("Start"));
    vbox->Add(startButton, 0, wxLEFT | wxTOP, 10);

    panel->SetSizer(vbox);
}

void MyFrame::OnRadioButtonSelected(wxCommandEvent& event)
{
    if (event.GetId() == ID_HumanRadioButton)
    {
        playerNameLabel->Show();
        playerNameTextCtrl->Show();
    }
    else if (event.GetId() == ID_BotRadioButton)
    {
        playerNameLabel->Hide();
        playerNameTextCtrl->Hide();
    }
    this->Layout();
}

void MyFrame::OnStartButtonClicked(wxCommandEvent& event)
{
    std::string playerName = (playerNameTextCtrl->GetValue()).ToStdString();
    int selectedRadioButtonID = humanRadioButton->GetValue() ? PlayerType::_HUMAN : PlayerType::_BOT;
    if (selectedRadioButtonID == PlayerType::_BOT)
    {
        playerName = "Bot";
    }
    
    std::cout<< "playerName = "<< playerName<<std::endl;
    std::cout<< "playerType = "<< selectedRadioButtonID<<std::endl;
    

    constexpr std::size_t kFramesPerSecond{60};
    constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
    constexpr std::size_t kScreenWidth{640};
    constexpr std::size_t kScreenHeight{640};
    constexpr std::size_t kGridWidth{32};
    constexpr std::size_t kGridHeight{32};

    //int player_type = PlayerType::_HUMAN;
    int player_type = selectedRadioButtonID;

    Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
    Controller controller;
    Game game(kGridWidth, kGridHeight);
    game.SetPlayerType(player_type);
    game.SetTmpPlayerName(std::move(playerName));

    game.Run(controller, renderer, kMsPerFrame);
    
    // Write or update human player's history to the file
    if(game.GetPlayerType() == PlayerType::_HUMAN)
        game.SaveToFile();
    std::cout<<"The current player's history was overwritten into the file."<<std::endl;
    
    std::cout << "Game has terminated successfully!\n";
    std::cout << "Score: " << game.GetScore() << "\n";
    std::cout << "Size: " << game.GetSize() << "\n";
}
