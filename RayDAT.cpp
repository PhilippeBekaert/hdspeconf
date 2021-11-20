/*! \file RayDAT.cpp
 *! \brief RME HDSPe RayDAT panel and control.
 * 20211120 - Philippe.Bekaert@uhasselt.be */

#include <thread>
#include <iostream>

#include "SndControl.h"
#include "RayDAT.h"
#include "RayDATPanel.h"

#include "HDSPeConf.h"

#define SET_CB(prop) card->prop.callOnValueChange(POSTCB(update_##prop,#prop))

class MyRayDATPanel: public RayDATPanel {
 protected:
  class RayDATCard* card { nullptr };

  constexpr static const double UNSET_PITCH { -1.0 };
  double newPitch = UNSET_PITCH;
  
 public:
  MyRayDATPanel(RayDATCard* _card, wxWindow* parent)
    : RayDATPanel(parent, wxID_ANY)
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
    
    SET_CB(spdifIn);
    SET_CB(spdifOpt);
    SET_CB(spdifPro);
    SET_CB(singleSpeedWclkOut);
    SET_CB(clrTms);
    SET_CB(adat1Internal);
    SET_CB(adat2Internal);    
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
    aesFreqLabel->SetLabelText(card->syncFreq.label(1));
    spdifFreqLabel->SetLabelText(card->syncFreq.label(2));
    adat1FreqLabel->SetLabelText(card->syncFreq.label(3));
    adat2FreqLabel->SetLabelText(card->syncFreq.label(4));
    adat3FreqLabel->SetLabelText(card->syncFreq.label(5));
    adat4FreqLabel->SetLabelText(card->syncFreq.label(6));    
    tcoFreqLabel->SetLabelText(card->syncFreq.label(7));
    syncInFreqLabel->SetLabelText(card->syncFreq.label(8));
    
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
    aesSyncButton->SetValue(false);
    spdifSyncButton->SetValue(false);
    adat1SyncButton->SetValue(false);
    adat2SyncButton->SetValue(false);
    adat3SyncButton->SetValue(false);
    adat4SyncButton->SetValue(false);    
    tcoSyncButton->SetValue(false);
    syncInSyncButton->SetValue(false);

    masterButton->SetValue(card->isMaster());
    
    if (!card->isMaster()) {
      switch (card->preferredRef) {
      case 0: wclkSyncButton->SetValue(true); break;
      case 1: aesSyncButton->SetValue(true); break;
      case 2: spdifSyncButton->SetValue(true); break;
      case 3: adat1SyncButton->SetValue(true); break;
      case 4: adat2SyncButton->SetValue(true); break;
      case 5: adat3SyncButton->SetValue(true); break;
      case 6: adat4SyncButton->SetValue(true); break;	
      case 7: tcoSyncButton->SetValue(true); break;
      case 8: syncInSyncButton->SetValue(true); break;	
      }
    }
  }

  void checkFreqs(void)
  {
    internalWarn->Show(card->internalRateDeviates());

    unsigned* f = card->syncFreq.values();
    wclkWarn->Show(!card->isClockCompatible(f[0]));
    aesWarn->Show(!card->isClockCompatible(f[1]));
    spdifWarn->Show(!card->isClockCompatible(f[2]));
    adat1Warn->Show(!card->isClockCompatible(f[3]));
    adat2Warn->Show(!card->isClockCompatible(f[4]));
    adat3Warn->Show(!card->isClockCompatible(f[5]));
    adat4Warn->Show(!card->isClockCompatible(f[6]));    
    tcoWarn->Show(!card->isClockCompatible(f[7]));
    syncInWarn->Show(!card->isClockCompatible(f[8]));
  }

  void enableSyncButtons(void)
  {
    unsigned* s = card->syncStatus.values();
    wclkSyncButton->Enable(s[0] != 3);
    aesSyncButton->Enable(s[1] != 3);
    spdifSyncButton->Enable(s[2] != 3);
    adat1SyncButton->Enable(s[3] != 3);
    adat2SyncButton->Enable(s[4] != 3);
    adat3SyncButton->Enable(s[5] != 3);
    adat4SyncButton->Enable(s[6] != 3);    
    tcoSyncButton->Enable(s[7] != 3);
    syncInSyncButton->Enable(s[8] != 3);    
  }

  void update_syncStatus(void)
  {
    wclkStatusLabel->SetLabelText(card->syncStatus.label(0));
    aesStatusLabel->SetLabelText(card->syncStatus.label(1));
    spdifStatusLabel->SetLabelText(card->syncStatus.label(2));
    adat1StatusLabel->SetLabelText(card->syncStatus.label(3));
    adat2StatusLabel->SetLabelText(card->syncStatus.label(4));
    adat3StatusLabel->SetLabelText(card->syncStatus.label(5));
    adat4StatusLabel->SetLabelText(card->syncStatus.label(6));    
    tcoStatusLabel->SetLabelText(card->syncStatus.label(7));
    syncInStatusLabel->SetLabelText(card->syncStatus.label(8));
    
    enableSyncButtons();
  }

  void update_adat1Internal(void)
  {
    adat1InternalButton->SetValue(card->adat1Internal);
  }

  void update_adat2Internal(void)
  {
    adat2InternalButton->SetValue(card->adat2Internal);
  }

  void update_spdifIn(void)
  {
    spdifInBox->SetSelection(card->spdifIn);
  }

  void update_spdifOpt(void)
  {
    spdifOpticalButton->SetValue(card->spdifOpt);
  }

  void update_spdifPro(void)
  {
    spdifProButton->SetValue(card->spdifPro);
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
  
  void aesSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(1);
    card->clockMode.set(0);
  }
  
  void spdifSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(2);
    card->clockMode.set(0);
  }
  
  void adat1SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(3);
    card->clockMode.set(0);
  }
  
  void adat2SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(4);
    card->clockMode.set(0);
  }
  
  void adat3SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(5);
    card->clockMode.set(0);
  }
  
  void adat4SyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(6);
    card->clockMode.set(0);
  }
  
  void tcoSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(7);
    card->clockMode.set(0);
  }
  
  void syncInSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(8);
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
  
  void spdifInCB(wxCommandEvent &event) override
  {
    card->spdifIn.set(event.GetInt());
  }
  
  void spdifOpticalCB(wxCommandEvent &event) override
  {
    card->spdifOpt.set(event.GetInt());
  }
  
  void spdifProfessionalCB(wxCommandEvent &event) override
  {
    card->spdifPro.set(event.GetInt());
  }
  
  void singleSpeedWclkOutCB(wxCommandEvent &event) override
  {
    card->singleSpeedWclkOut.set(event.GetInt());
  }
  
  void tmsCB(wxCommandEvent &event) override
  {
    card->clrTms.set(!event.GetInt());
  }

  void adat1InternalCB(wxCommandEvent &event) override
  {
    card->adat1Internal.set(event.GetInt());
  }

  void adat2InternalCB(wxCommandEvent &event) override
  {
    card->adat2Internal.set(event.GetInt());
  }
};

RayDATCard::RayDATCard(int index)
  : HDSPeCard(index)
  , spdifIn(this, "S/PDIF In")
  , spdifOpt(this, "S/PDIF Out Optical")
  , spdifPro(this, "S/PDIF Out Professional")
  , singleSpeedWclkOut(this, "Single Speed WordClk Out")
  , clrTms(this, "Clear TMS")
  , adat1Internal(this, "ADAT1 Internal")
  , adat2Internal(this, "ADAT2 Internal")    
{
  modelName = "RayDAT";
  tcoSyncChoice = 7;
}

RayDATCard::~RayDATCard()
{
}

class wxPanel* RayDATCard::makePanel(wxWindow* parent)
{
  return panel = new MyRayDATPanel(this, parent);
}
