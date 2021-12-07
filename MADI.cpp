/*! \file MADI.cpp
 *! \brief RME HDSPe MADI panel and control.
 * 20211207 - Philippe.Bekaert@uhasselt.be */

#include <thread>
#include <iostream>

#include "SndControl.h"
#include "MADI.h"
#include "MADIPanel.h"

#include "HDSPeConf.h"

#define SET_CB(prop) card->prop.callOnValueChange(POSTCB(update_##prop,#prop))

class MyMADIPanel: public MADIPanel {
 protected:
  class MADICard* card { nullptr };

  constexpr static const double UNSET_PITCH { -1.0 };
  double newPitch = UNSET_PITCH;
  
 public:
  MyMADIPanel(MADICard* _card, wxWindow* parent)
    : MADIPanel(parent, wxID_ANY)
    , card(_card)
  {
    fwVersionLabel->SetLabelText(std::to_string(card->fwBuild));
    
    SET_CB(running);
    SET_CB(bufferSize);
    SET_CB(clockMode);
    SET_CB(internalFreq);
    SET_CB(preferredRef);
    SET_CB(syncRef);
    SET_CB(syncStatus);
    SET_CB(syncFreq);
    SET_CB(sampleRate);

    SET_CB(externalFreq);
    SET_CB(preferredInput);
    SET_CB(currentInput);
    SET_CB(autoselectInput);
    SET_CB(rx64ch);
    SET_CB(tx64ch);
    SET_CB(doubleWire);
    SET_CB(singleSpeedWclkOut);
    SET_CB(clrTms);

    currentMadiInputBox->Enable(false);
  }

  void update_running(void)
  {
    internalFreqLabel->Show(card->running);
    internalFreqLabel->SetLabelText(card->internalFreq.label());
    
    internalFreqChoice->Show(!card->running);
    internalFreqChoice->SetSelection(card->internalFreq.value());    
  }

  void update_bufferSize(void)
  {
    bufferSizeLabel->SetLabelText(std::to_string(card->bufferSize));
  }

  void update_clockMode(void)
  {
    setClockSourceLabel();
    setSyncButtonState();
  }

  void update_syncRef(void)
  {
    setClockSourceLabel();
  }

  void update_preferredRef(void)
  {
    setSyncButtonState();
  }
  
  void update_internalFreq(void)
  {
    internalFreqLabel->SetLabelText(card->internalFreq.label());
    internalFreqChoice->SetSelection(card->internalFreq.value());
    
    checkFreqs();
  }
  
  void update_syncFreq(void)
  {
    wclkFreqLabel->SetLabelText(card->syncFreq.label(0));
    madiFreqLabel->SetLabelText(card->syncFreq.label(1));
    tcoFreqLabel->SetLabelText(card->syncFreq.label(2));
    syncInFreqLabel->SetLabelText(card->syncFreq.label(3));
    
    checkFreqs();
  }

  void update_sampleRate(void)
  {
    int rate = (int)round(card->getSystemSampleRate());
    sampleRateLabel->SetLabelText(std::to_string(rate));
    sampleRateLabel->SetBackgroundColour(card->isStandardSampleRate(rate)
					 ? wxNullColour : wxColour(0xff, 0xc6, 0x00));

    pitchSlider->Enable(card->isMaster());
    pitchSlider->SetValue(card->getPitch() * 1e6);  // display pitch in PPM

    checkFreqs();
  }

  void setClockSourceLabel(void)
  {
    clockSourceLabel->SetLabelText(card->isMaster() ? "Master" :
				   card->syncRef.label());    
  }
    
  void setSyncButtonState(void)
  {
    wclkSyncButton->SetValue(false);
    madiSyncButton->SetValue(false);
    tcoSyncButton->SetValue(false);
    syncInSyncButton->SetValue(false);

    masterButton->SetValue(card->isMaster());
    
    if (!card->isMaster()) {
      switch (card->preferredRef) {
      case 0: wclkSyncButton->SetValue(true); break;
      case 1: madiSyncButton->SetValue(true); break;
      case 2: tcoSyncButton->SetValue(true); break;
      case 3: syncInSyncButton->SetValue(true); break;	
      }
    }
  }

  void checkFreqs(void)
  {
    internalWarn->Show(card->internalRateDeviates());

    unsigned* f = card->syncFreq.values();
    wclkWarn->Show(!card->isClockCompatible(f[0]));
    madiWarn->Show(!card->isClockCompatible(f[1]));
    tcoWarn->Show(!card->isClockCompatible(f[2]));
    syncInWarn->Show(!card->isClockCompatible(f[3]));
  }

  void enableSyncButtons(void)
  {
    unsigned* s = card->syncStatus.values();
    wclkSyncButton->Enable(s[0] != 3);
    madiSyncButton->Enable(s[1] != 3);
    tcoSyncButton->Enable(s[2] != 3);
    syncInSyncButton->Enable(s[3] != 3);
  }

  void update_syncStatus(void)
  {
    wclkStatusLabel->SetLabelText(card->syncStatus.label(0));
    madiStatusLabel->SetLabelText(card->syncStatus.label(1));
    tcoStatusLabel->SetLabelText(card->syncStatus.label(2));
    syncInStatusLabel->SetLabelText(card->syncStatus.label(3));
    
    enableSyncButtons();
  }

  void update_externalFreq(void)
  {
    //    std::cerr << __func__ << ": externalFreq=" << card->externalFreq << "\n";
  }

  void update_preferredInput(void)
  {
    madiInputBox->SetSelection(card->preferredInput.value());
  }

  void update_currentInput(void)
  {
    currentMadiInputBox->SetSelection(card->currentInput.value());
  }

  void update_autoselectInput(void)
  {
    autoselectInputButton->SetValue(card->autoselectInput.value());
  }

  void update_rx64ch(void)
  {
    rx64chButton->SetValue(card->rx64ch.value());
  }

  void update_tx64ch(void)
  {
    tx64chButton->SetValue(card->tx64ch.value());
  }

  void update_doubleWire(void)
  {
    doubleWireButton->SetValue(card->doubleWire.value());
  }

  void update_singleSpeedWclkOut(void)
  {
    if (card->hasTco()) {  /* TCO is always single speed word clock out */
      singleSpeedWclkButton->Disable();
      singleSpeedWclkButton->SetValue(true);
    } else {
      singleSpeedWclkButton->Enable();
      singleSpeedWclkButton->SetValue(card->singleSpeedWclkOut);
    }
  }

  void update_clrTms(void)
  {
    tmsButton->SetValue(!card->clrTms);
  }

  void internalFreqCB(wxCommandEvent &event) override
  {
    card->internalFreq.set(event.GetInt());
  }

  void masterCB(wxCommandEvent &event) override
  {
    card->clockMode.set(1);
  }

  void wclkSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(0);
    card->clockMode.set(0);
  }
  
  void madiSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(1);
    card->clockMode.set(0);
  }
    
  void tcoSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(2);
    card->clockMode.set(0);
  }
  
  void syncInSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(3);
    card->clockMode.set(0);
  }
  
  void pitchDownCB(wxScrollEvent &event) override
  {
    newPitch = card->upPitch();  // no mistake ... arrow buttons are reversed
  }
  
  void pitchUpCB(wxScrollEvent &event) override
  {
    newPitch = card->downPitch();
  }
  
  void pitchPrevCB(wxScrollEvent &event) override
  {
    newPitch = card->prevPitch();
  }
  
  void pitchNextCB(wxScrollEvent &event) override
  {
    newPitch = card->nextPitch();
  }
  
  void pitchSliderCB(wxCommandEvent &event) override
  {
    if (newPitch == UNSET_PITCH)
      newPitch = (double)event.GetInt() * 1e-6;
    else
      pitchSlider->SetValue(newPitch * 1e6);
    card->setPitch(newPitch);
    newPitch = UNSET_PITCH;
  }

  void madiInputCB(wxCommandEvent &event) override
  {
    card->preferredInput.set(event.GetInt());
  }

  void autoselectInputCB(wxCommandEvent &event) override
  {
    card->autoselectInput.set(event.GetInt());
  }

  void tx64chCB(wxCommandEvent &event) override
  {
    card->tx64ch.set(event.GetInt());
  }

  void doubleWireCB(wxCommandEvent& event) override
  {
    card->doubleWire.set(event.GetInt());
  }  
  
  void singleSpeedWclkOutCB(wxCommandEvent &event) override
  {
    card->singleSpeedWclkOut.set(event.GetInt());
  }
  
  void tmsCB(wxCommandEvent &event) override
  {
    card->clrTms.set(!event.GetInt());
  }
};

MADICard::MADICard(int index)
  : HDSPeCard(index)
  , externalFreq(this, "External Frequency")
  , preferredInput(this, "Preferred Input")
  , currentInput(this, "Current Input")
  , autoselectInput(this, "Autoselect Input")
  , rx64ch(this, "RX 64 Channels Mode")
  , tx64ch(this, "TX 64 Channels Mode")
  , doubleWire(this, "Double Wire Mode")
  , singleSpeedWclkOut(this, "Single Speed WordClk Out")
  , clrTms(this, "Clear TMS")
{
  modelName = "MADI";
  tcoSyncChoice = 2;
}

MADICard::~MADICard()
{
}

class wxPanel* MADICard::makePanel(wxWindow* parent)
{
  return panel = new MyMADIPanel(this, parent);
}
