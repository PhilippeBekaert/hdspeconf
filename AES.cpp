/*! \file AES.cpp
 *! \brief RME HDSPe AES panel and control.
 * 20211120 - Philippe.Bekaert@uhasselt.be */

#include <thread>
#include <iostream>

#include "SndControl.h"
#include "AES.h"
#include "AESPanel.h"

#include "HDSPeConf.h"

#define SET_CB(prop) card->prop.callOnValueChange(POSTCB(update_##prop,#prop))

class MyAESPanel: public AESPanel {
 protected:
  class AESCard* card { nullptr };

  constexpr static const double UNSET_PITCH { -1.0 };
  double newPitch = UNSET_PITCH;
  
 public:
  MyAESPanel(AESCard* _card, wxWindow* parent)
    : AESPanel(parent, wxID_ANY)
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
    
    SET_CB(doubleSpeedMode);
    SET_CB(quadSpeedMode);
    SET_CB(professional);
    SET_CB(emphasis);
    SET_CB(nonAudio);
    SET_CB(singleSpeedWclkOut);
    SET_CB(clrTms);
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
    aes1FreqLabel->SetLabelText(card->syncFreq.label(1));
    aes2FreqLabel->SetLabelText(card->syncFreq.label(2));
    aes3FreqLabel->SetLabelText(card->syncFreq.label(3));
    aes4FreqLabel->SetLabelText(card->syncFreq.label(4));    
    aes5FreqLabel->SetLabelText(card->syncFreq.label(5));
    aes6FreqLabel->SetLabelText(card->syncFreq.label(6));
    aes7FreqLabel->SetLabelText(card->syncFreq.label(7));
    aes8FreqLabel->SetLabelText(card->syncFreq.label(8));    
    tcoFreqLabel->SetLabelText(card->syncFreq.label(9));
    syncInFreqLabel->SetLabelText(card->syncFreq.label(10));
    
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
    aes1SyncButton->SetValue(false);
    aes2SyncButton->SetValue(false);    
    aes3SyncButton->SetValue(false);
    aes4SyncButton->SetValue(false);    
    aes5SyncButton->SetValue(false);
    aes6SyncButton->SetValue(false);    
    aes7SyncButton->SetValue(false);
    aes8SyncButton->SetValue(false);    
    tcoSyncButton->SetValue(false);
    syncInSyncButton->SetValue(false);

    masterButton->SetValue(card->isMaster());
    
    if (!card->isMaster()) {
      switch (card->preferredRef) {
      case 0: wclkSyncButton->SetValue(true); break;
      case 1: aes1SyncButton->SetValue(true); break;
      case 2: aes2SyncButton->SetValue(true); break;	
      case 3: aes3SyncButton->SetValue(true); break;
      case 4: aes4SyncButton->SetValue(true); break;	
      case 5: aes5SyncButton->SetValue(true); break;
      case 6: aes6SyncButton->SetValue(true); break;	
      case 7: aes7SyncButton->SetValue(true); break;
      case 8: aes8SyncButton->SetValue(true); break;	
      case 9: tcoSyncButton->SetValue(true); break;
      case 10: syncInSyncButton->SetValue(true); break;	
      }
    }
  }

  void checkFreqs(void)
  {
    internalWarn->Show(card->internalRateDeviates());

    unsigned* f = card->syncFreq.values();
    wclkWarn->Show(!card->isClockCompatible(f[0]));
    aes1Warn->Show(!card->isClockCompatible(f[1]));
    aes2Warn->Show(!card->isClockCompatible(f[2]));    
    aes3Warn->Show(!card->isClockCompatible(f[3]));
    aes4Warn->Show(!card->isClockCompatible(f[4]));    
    aes5Warn->Show(!card->isClockCompatible(f[5]));
    aes6Warn->Show(!card->isClockCompatible(f[6]));    
    aes7Warn->Show(!card->isClockCompatible(f[7]));
    aes8Warn->Show(!card->isClockCompatible(f[8]));    
    tcoWarn->Show(!card->isClockCompatible(f[9]));
    syncInWarn->Show(!card->isClockCompatible(f[10]));
  }

  void enableSyncButtons(void)
  {
    unsigned* s = card->syncStatus.values();
    wclkSyncButton->Enable(s[0] != 3);
    aes1SyncButton->Enable(s[1] != 3);
    aes2SyncButton->Enable(s[2] != 3);    
    aes3SyncButton->Enable(s[3] != 3);
    aes4SyncButton->Enable(s[4] != 3);    
    aes5SyncButton->Enable(s[5] != 3);
    aes6SyncButton->Enable(s[6] != 3);    
    aes7SyncButton->Enable(s[7] != 3);
    aes8SyncButton->Enable(s[8] != 3);    
    tcoSyncButton->Enable(s[9] != 3);
    syncInSyncButton->Enable(s[10] != 3);    
  }

  void update_syncStatus(void)
  {
    wclkStatusLabel->SetLabelText(card->syncStatus.label(0));
    aes1StatusLabel->SetLabelText(card->syncStatus.label(1));
    aes2StatusLabel->SetLabelText(card->syncStatus.label(2));    
    aes3StatusLabel->SetLabelText(card->syncStatus.label(3));
    aes4StatusLabel->SetLabelText(card->syncStatus.label(4));    
    aes5StatusLabel->SetLabelText(card->syncStatus.label(5));
    aes6StatusLabel->SetLabelText(card->syncStatus.label(6));    
    aes7StatusLabel->SetLabelText(card->syncStatus.label(7));
    aes8StatusLabel->SetLabelText(card->syncStatus.label(8));    
    tcoStatusLabel->SetLabelText(card->syncStatus.label(9));
    syncInStatusLabel->SetLabelText(card->syncStatus.label(10));
    
    enableSyncButtons();
  }

  void update_doubleSpeedMode(void)
  {
    dsModeBox->SetSelection(card->doubleSpeedMode);
  }

  void update_quadSpeedMode(void)
  {
    qsModeBox->SetSelection(card->quadSpeedMode);
  }

  void update_professional(void)
  {
    professionalButton->SetValue(card->professional);
  }

  void update_emphasis(void)
  {
    emphasisButton->SetValue(card->emphasis);
  }

  void update_nonAudio(void)
  {
    nonAudioButton->SetValue(card->nonAudio);
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
  
  void aes1SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(1);
    card->clockMode.set(0);
  }
  
  void aes2SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(2);
    card->clockMode.set(0);
  }
  
  void aes3SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(3);
    card->clockMode.set(0);
  }
  
  void aes4SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(4);
    card->clockMode.set(0);
  }
  
  void aes5SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(5);
    card->clockMode.set(0);
  }
  
  void aes6SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(6);
    card->clockMode.set(0);
  }
  
  void aes7SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(7);
    card->clockMode.set(0);
  }
  
  void aes8SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(8);
    card->clockMode.set(0);
  }
  
  void tcoSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(9);
    card->clockMode.set(0);
  }
  
  void syncInSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(10);
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

  void professionalCB(wxCommandEvent &event) override
  {
    card->professional.set(event.GetInt());
  }
  
  void emphasisCB(wxCommandEvent &event) override
  {
    card->emphasis.set(event.GetInt());
  }
  
  void nonAudioCB(wxCommandEvent &event) override
  {
    card->nonAudio.set(event.GetInt());
  }
  
  void singleSpeedWclkOutCB(wxCommandEvent &event) override
  {
    card->singleSpeedWclkOut.set(event.GetInt());
  }
  
  void tmsCB(wxCommandEvent &event) override
  {
    card->clrTms.set(!event.GetInt());
  }

  void dsModeCB(wxCommandEvent &event) override
  {
    card->doubleSpeedMode.set(event.GetInt());
  }

  void qsModeCB(wxCommandEvent &event) override
  {
    card->quadSpeedMode.set(event.GetInt());
  }
};

AESCard::AESCard(int index)
  : HDSPeCard(index)
  , doubleSpeedMode(this, "Double Speed Mode")
  , quadSpeedMode(this, "Quad Speed Mode")
  , professional(this, "Professional")
  , emphasis(this, "Emphasis")
  , nonAudio(this, "Non Audio")
  , singleSpeedWclkOut(this, "Single Speed WordClk Out")
  , clrTms(this, "Clear TMS")
{
  modelName = "AES";
  tcoSyncChoice = 9;
}

AESCard::~AESCard()
{
}

class wxPanel* AESCard::makePanel(wxWindow* parent)
{
  return panel = new MyAESPanel(this, parent);
}
