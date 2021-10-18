/*! \file hdspeconf.cpp
 *! \brief hdspeconf main.
 * 20210809,10,11,12,0907,08,15,16 - Philippe.Bekaert@uhasselt.be */

// TODO: catch runtime errors in a Error dialog.

#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <sys/types.h>

#include <wx/choicebk.h>

#include "NoCardsPanel.h"

#include "HDSPeConf.h"
#include "HDSPeCard.h"

//! \brief Main window: a notebook containing pages for each HDSPe card
//! and TCO.
class MainWindow: public wxFrame {
public:
  HDSPeCardEnumerator cardEnumerator;  // enumerates HDSPe cards on construction

    MainWindow()    
    : wxFrame(nullptr, wxID_ANY, wxEmptyString,
	      wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)
  {
    SetTitle(wxT("hdspeconf"));
    wxPanel* panel_1 = new wxPanel(this, wxID_ANY);
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    notebook_1 = new wxChoicebook(panel_1, wxID_ANY);
    sizer_1->Add(notebook_1, 1, wxEXPAND, 0);

    // Add a notebook page for each hdspe card + TCO expansion module.
    std::vector<HDSPeCard*>& cards = cardEnumerator.getCards();
    if (cards.size() == 0) {
      // Add page with message if no hdspe driven cards are available
      notebook_1->AddPage(new NoCardsPanel(notebook_1, wxID_ANY), wxT(""));
    } else {
      for (auto card: cards) {
	wxPanel *panel = card->makePanel(notebook_1);
	notebook_1->AddPage(panel, card->getPrettyName());

	if (card->hasTco())
	  notebook_1->AddPage(card->makeTcoPanel(notebook_1),
			      std::string(card->getPrettyName()) + " TCO");
      }
    }
    // TODO: add "About" panel.
    
    panel_1->SetSizer(sizer_1);
    sizer_1->Fit(panel_1);
    Layout();

    // Need to set proper initial and minimal window size ourselves, it seems.
    wxSize sz = panel_1->GetBestSize();
    int h = sz.GetHeight(), w = sz.GetWidth();
    sz.SetHeight(h < 300 ? 300 : h+48);
    sz.SetWidth(w < 400 ? 400 : w+16);
    SetInitialSize(ClientToWindowSize(sz));
  }

  ~MainWindow()
  {
  }

protected:
  wxChoicebook *notebook_1 { nullptr };
};

//! \brief The application.
class HDSPeConf: public wxApp {
public:
  bool OnInit()
  {
    wxInitAllImageHandlers();
    
    try {
      mainWindow = new MainWindow;      
      SetTopWindow(mainWindow);

      mainWindow->Show();
    } catch (std::runtime_error &e) {
      std::cerr << e.what() << "\n";
      return false;
    }
    return true;
  }

  int OnExit()
  {
    return 0;
  }

  //! \brief Post callback function.
  void post(std::function<void(void)> cb)
  {
    CallAfter(cb);
  }

protected:
  MainWindow* mainWindow {nullptr};
};

IMPLEMENT_APP(HDSPeConf)

void PostCB(std::function<void(void)> cb)
{
  ::wxGetApp().post(cb);
}

#ifdef NEVER
#include <time.h>
double GetTime(void)
{
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC_RAW, &t);
  double time = (double)t.tv_sec + (double)t.tv_nsec * 1e-9;

  static double starttime = 0.0;
  if (starttime == 0.0)
    starttime = time;
  return time - starttime;
}
#endif /*NEVER*/
