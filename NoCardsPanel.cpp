// -*- C++ -*-
//
// generated by wxGlade 1.0.0 on Tue Aug 10 18:59:00 2021
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#include "NoCardsPanel.h"

// begin wxGlade: ::extracode
// end wxGlade



NoCardsPanel::NoCardsPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style):
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
  // begin wxGlade: NoCardsPanel::NoCardsPanel
  wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
  sizer_1->Add(sizer_2, 1, wxALIGN_CENTER_HORIZONTAL, 0);
  wxStaticText* label_1 = new wxStaticText(this, wxID_ANY, wxT("No supported cards detected."));
  sizer_2->Add(label_1, 0, wxALIGN_CENTER_VERTICAL, 0);
  
  SetSizer(sizer_1);
  sizer_1->Fit(this);
  // end wxGlade
}

