// -*- C++ -*-
//
// generated by wxGlade 1.0.3 on Wed Nov 17 19:53:33 2021
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#include "TCOPanel.h"

// begin wxGlade: ::extracode
// end wxGlade



TCOPanel::TCOPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style):
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
  // begin wxGlade: TCOPanel::TCOPanel
  wxBoxSizer* sizer_7 = new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer* sizer_1 = new wxFlexGridSizer(4, 1, 0, 0);
  sizer_7->Add(sizer_1, 0, wxALIGN_CENTER_HORIZONTAL|wxFIXED_MINSIZE, 0);
  wxStaticBoxSizer* sizer_2 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Sync Source / Input Status")), wxVERTICAL);
  sizer_1->Add(sizer_2, 1, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 4);
  wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer(3, 3, 0, 0);
  sizer_2->Add(grid_sizer_1, 0, wxEXPAND, 0);
  ltcSyncButton = new wxRadioButton(this, ltcSyncID, wxT("LTC"));
  grid_sizer_1->Add(ltcSyncButton, 0, wxLEFT|wxRIGHT, 4);
  ltcStatusLabel = new wxStaticText(this, wxID_ANY, wxT("12:23:34:00"));
  grid_sizer_1->Add(ltcStatusLabel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);
  ltcInFrameRateLabel = new wxStaticText(this, wxID_ANY, wxT("29.97 dfps"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  grid_sizer_1->Add(ltcInFrameRateLabel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 20);
  videoSyncButton = new wxRadioButton(this, videoSyncID, wxT("Video"));
  videoSyncButton->Enable(0);
  grid_sizer_1->Add(videoSyncButton, 0, wxLEFT|wxRIGHT, 4);
  videoStatusLabel = new wxStaticText(this, wxID_ANY, wxT("No Video"));
  grid_sizer_1->Add(videoStatusLabel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);
  wxStaticText* label_5 = new wxStaticText(this, wxID_ANY, wxEmptyString);
  grid_sizer_1->Add(label_5, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);
  wckSyncButton = new wxRadioButton(this, wckSyncID, wxT("WordClk"));
  grid_sizer_1->Add(wckSyncButton, 0, wxLEFT|wxRIGHT, 4);
  wckStatusLabel = new wxStaticText(this, wxID_ANY, wxT("Single Speed"));
  grid_sizer_1->Add(wckStatusLabel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);
  termButton = new wxCheckBox(this, termID, wxT("75 Ohm Term."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
  grid_sizer_1->Add(termButton, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxLEFT, 16);
  wxBoxSizer* sizer_3 = new wxBoxSizer(wxHORIZONTAL);
  sizer_1->Add(sizer_3, 1, wxEXPAND, 0);
  wxBoxSizer* sizer_4 = new wxBoxSizer(wxVERTICAL);
  sizer_3->Add(sizer_4, 1, wxEXPAND, 0);
  const wxString ltcFrameRateBox_choices[] = {
    wxT("24 fps"),
    wxT("25 fps"),
    wxT("29.97 fps"),
    wxT("30 fps"),
  };
  ltcFrameRateBox = new wxRadioBox(this, ltcFrameRateID, wxT("LTC Frame Rate"), wxDefaultPosition, wxDefaultSize, 4, ltcFrameRateBox_choices, 1, wxRA_SPECIFY_COLS);
  ltcFrameRateBox->SetSelection(0);
  sizer_4->Add(ltcFrameRateBox, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 4);
  dropFrameButton = new wxCheckBox(this, dropFrameID, wxT("Drop Frame LTC"));
  sizer_4->Add(dropFrameButton, 0, wxALL|wxEXPAND, 4);
  const wxString wckConversionBox_choices[] = {
    wxT("1:1"),
    wxT("44.1 KHz -> 48 KHz"),
    wxT("48 KHz -> 44.1 KHz"),
  };
  wckConversionBox = new wxRadioBox(this, wckConversionID, wxT("WordClk Conversion"), wxDefaultPosition, wxDefaultSize, 3, wckConversionBox_choices, 1, wxRA_SPECIFY_COLS);
  wckConversionBox->SetSelection(0);
  sizer_4->Add(wckConversionBox, 0, wxALL|wxEXPAND, 4);
  wxBoxSizer* sizer_5 = new wxBoxSizer(wxVERTICAL);
  sizer_3->Add(sizer_5, 1, wxEXPAND, 0);
  const wxString ltcSampleRateBox_choices[] = {
    wxT("44.1 KHz"),
    wxT("48 KHz"),
    wxT("From App"),
  };
  ltcSampleRateBox = new wxRadioBox(this, ltcSampleRateID, wxT("LTC Sample Rate"), wxDefaultPosition, wxDefaultSize, 3, ltcSampleRateBox_choices, 1, wxRA_SPECIFY_COLS);
  ltcSampleRateBox->SetSelection(0);
  sizer_5->Add(ltcSampleRateBox, 0, wxALL|wxEXPAND, 4);
  sizer_5->Add(20, 1, 0, 0, 0);
  const wxString pullBox_choices[] = {
    wxT("None"),
    wxT("+0.1 %"),
    wxT(" -0.1 %"),
    wxT("+4 %"),
    wxT("- 4 %"),
  };
  pullBox = new wxRadioBox(this, pullID, wxT("Pull Up/Down"), wxDefaultPosition, wxDefaultSize, 5, pullBox_choices, 1, wxRA_SPECIFY_COLS);
  pullBox->SetSelection(0);
  sizer_5->Add(pullBox, 0, wxALL|wxEXPAND, 4);
  wxBoxSizer* sizer_9 = new wxBoxSizer(wxHORIZONTAL);
  sizer_1->Add(sizer_9, 1, wxALL|wxEXPAND, 4);
  wxStaticBoxSizer* testerBox = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("System Sample Rate")), wxHORIZONTAL);
  sizer_9->Add(testerBox, 1, 0, 0);
  useTCOButton = new wxCheckBox(this, useTcoID, wxT("Use TCO"));
  testerBox->Add(useTCOButton, 0, wxALL|wxEXPAND, 4);
  sampleRateLabel = new wxStaticText(this, wxID_ANY, wxT("47519"));
  testerBox->Add(sampleRateLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
  lockLabel = new wxStaticText(this, wxID_ANY, wxT("No TCO Lock"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
  testerBox->Add(lockLabel, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 12);
  autoButton = new wxButton(this, autoID, wxT("Auto"));
  testerBox->Add(autoButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
  wxStaticBoxSizer* sizer_6 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("LTC Out")), wxHORIZONTAL);
  sizer_1->Add(sizer_6, 1, wxALL|wxEXPAND, 4);
  wxBoxSizer* sizer_8 = new wxBoxSizer(wxVERTICAL);
  sizer_6->Add(sizer_8, 1, wxEXPAND, 0);
  wxBoxSizer* sizer_10 = new wxBoxSizer(wxHORIZONTAL);
  sizer_8->Add(sizer_10, 1, wxEXPAND, 0);
  positionalButton = new wxButton(this, positionalID, wxT("Positional"));
  sizer_10->Add(positionalButton, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4);
  wallClockButton = new wxButton(this, wallClockID, wxT("Real Time"));
  sizer_10->Add(wallClockButton, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4);
  jamSyncButton = new wxButton(this, jamSyncID, wxT("Jam Sync"));
  sizer_10->Add(jamSyncButton, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4);
  ltcRunButton = new wxCheckBox(this, runID, wxT("Run"));
  sizer_8->Add(ltcRunButton, 1, wxALL, 4);
  
  SetSizer(sizer_7);
  sizer_7->Fit(this);
  // end wxGlade
}


BEGIN_EVENT_TABLE(TCOPanel, wxPanel)
  // begin wxGlade: TCOPanel::event_table
  EVT_RADIOBUTTON(ltcSyncID, TCOPanel::ltcSyncCB)
  EVT_RADIOBUTTON(videoSyncID, TCOPanel::videoSyncCB)
  EVT_RADIOBUTTON(wckSyncID, TCOPanel::wckSyncCB)
  EVT_CHECKBOX(termID, TCOPanel::termCB)
  EVT_RADIOBOX(ltcFrameRateID, TCOPanel::ltcFrameRateCB)
  EVT_CHECKBOX(dropFrameID, TCOPanel::dropFrameCB)
  EVT_RADIOBOX(wckConversionID, TCOPanel::wckConversionCB)
  EVT_RADIOBOX(ltcSampleRateID, TCOPanel::ltcSampleRateCB)
  EVT_RADIOBOX(pullID, TCOPanel::pullCB)
  EVT_CHECKBOX(useTcoID, TCOPanel::useTcoCB)
  EVT_BUTTON(autoID, TCOPanel::autoCB)
  EVT_BUTTON(positionalID, TCOPanel::positionalCB)
  EVT_BUTTON(wallClockID, TCOPanel::wallClockCB)
  EVT_BUTTON(jamSyncID, TCOPanel::jamSyncCB)
  EVT_CHECKBOX(runID, TCOPanel::ltcRunCB)
  // end wxGlade
END_EVENT_TABLE();


void TCOPanel::ltcSyncCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::ltcSyncCB) not implemented yet"));
}

void TCOPanel::videoSyncCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::videoSyncCB) not implemented yet"));
}

void TCOPanel::wckSyncCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::wckSyncCB) not implemented yet"));
}

void TCOPanel::termCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::termCB) not implemented yet"));
}

void TCOPanel::ltcFrameRateCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::ltcFrameRateCB) not implemented yet"));
}

void TCOPanel::dropFrameCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::dropFrameCB) not implemented yet"));
}

void TCOPanel::wckConversionCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::wckConversionCB) not implemented yet"));
}

void TCOPanel::ltcSampleRateCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::ltcSampleRateCB) not implemented yet"));
}

void TCOPanel::pullCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::pullCB) not implemented yet"));
}

void TCOPanel::useTcoCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::useTcoCB) not implemented yet"));
}

void TCOPanel::autoCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::autoCB) not implemented yet"));
}

void TCOPanel::positionalCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::positionalCB) not implemented yet"));
}

void TCOPanel::wallClockCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::wallClockCB) not implemented yet"));
}

void TCOPanel::jamSyncCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::jamSyncCB) not implemented yet"));
}

void TCOPanel::ltcRunCB(wxCommandEvent &event)  // wxGlade: TCOPanel.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (TCOPanel::ltcRunCB) not implemented yet"));
}


// wxGlade: add TCOPanel event handlers

