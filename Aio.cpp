/*! \file Aio.cpp
 *! \brief RME HDSPe Aio panel and control.
 * 20211117,19 - Philippe.Bekaert@uhasselt.be */

#include <thread>
#include <iostream>

#include "SndControl.h"
#include "Aio.h"
#include "AioPanel.h"

#include "HDSPeConf.h"

#define SET_CB(prop) card->prop.callOnValueChange(POSTCB(update_##prop,#prop))

class MyAioPanel: public AioPanel {
 protected:
  class AioCard* card { nullptr };

  constexpr static const double UNSET_PITCH { -1.0 };
  double newPitch = UNSET_PITCH;
  
 public:
  MyAioPanel(AioCard* _card, wxWindow* parent)
    : AioPanel(parent, wxID_ANY)
    , card(_card)
  {
    fwVersionLabel->SetLabelText(std::to_string(card->fwBuild));
    ao4sButton->SetValue(card->ao4s);
    ai4sButton->SetValue(card->ai4s);
    tcoButton->SetValue(card->tcoPresent);
    
    SET_CB(running);
    SET_CB(bufferSize);
    SET_CB(clockMode);
    SET_CB(internalFreq);
    SET_CB(preferredRef);
    SET_CB(syncRef);
    SET_CB(syncStatus);
    SET_CB(syncFreq);
    SET_CB(sampleRate);
    
    SET_CB(inputLevel);
    SET_CB(outputLevel);
    SET_CB(phonesLevel);
    SET_CB(spdifIn);
    SET_CB(spdifOpt);
    SET_CB(spdifPro);
    SET_CB(singleSpeedWclkOut);
    SET_CB(clrTms);
    SET_CB(xlr);
    SET_CB(adatInternal);
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
    adatFreqLabel->SetLabelText(card->syncFreq.label(3));
    tcoFreqLabel->SetLabelText(card->syncFreq.label(4));
    syncInFreqLabel->SetLabelText(card->syncFreq.label(5));
    
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

  void update_adatInternal(void)
  {
    adatInternalButton->SetValue(card->adatInternal);
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
    adatSyncButton->SetValue(false);
    tcoSyncButton->SetValue(false);
    syncInSyncButton->SetValue(false);

    masterButton->SetValue(card->isMaster());
    
    if (!card->isMaster()) {
      switch (card->preferredRef) {
      case 0: wclkSyncButton->SetValue(true); break;
      case 1: aesSyncButton->SetValue(true); break;
      case 2: spdifSyncButton->SetValue(true); break;
      case 3: adatSyncButton->SetValue(true); break;
      case 4: tcoSyncButton->SetValue(true); break;
      case 5: syncInSyncButton->SetValue(true); break;	
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
    adatWarn->Show(!card->isClockCompatible(f[3]));
    tcoWarn->Show(!card->isClockCompatible(f[4]));
    syncInWarn->Show(!card->isClockCompatible(f[5]));
  }

  void enableSyncButtons(void)
  {
    unsigned* s = card->syncStatus.values();
    wclkSyncButton->Enable(s[0] != 3);
    aesSyncButton->Enable(s[1] != 3);
    spdifSyncButton->Enable(s[2] != 3);
    adatSyncButton->Enable(s[3] != 3);
    tcoSyncButton->Enable(s[4] != 3);
    syncInSyncButton->Enable(s[5] != 3);    
  }

  void update_syncStatus(void)
  {
    wclkStatusLabel->SetLabelText(card->syncStatus.label(0));
    aesStatusLabel->SetLabelText(card->syncStatus.label(1));
    spdifStatusLabel->SetLabelText(card->syncStatus.label(2));
    adatStatusLabel->SetLabelText(card->syncStatus.label(3));
    tcoStatusLabel->SetLabelText(card->syncStatus.label(4));
    syncInStatusLabel->SetLabelText(card->syncStatus.label(5));
    
    enableSyncButtons();
  }
  
  void update_inputLevel(void)
  {
    inputLevelBox->SetSelection(card->inputLevel);
  }

  void update_outputLevel(void)
  {
    outputLevelBox->SetSelection(card->outputLevel);
  }

  void update_xlr(void)
  {
    // 1 - level because the buttons are declared bottom-to-top
    analogOutBox->SetSelection(1 - card->xlr);
  }

  void update_phonesLevel(void)
  {
    phonesLevelBox->SetSelection(card->phonesLevel);
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
  
  void adatSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(3);
    card->clockMode.set(0);
  }
  
  void tcoSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(4);
    card->clockMode.set(0);
  }
  
  void syncInSyncCB(wxCommandEvent &event) override
  {
    card->preferredRef.set(5);
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
  
  void inputLevelCB(wxCommandEvent &event) override
  {
    card->inputLevel.set(event.GetInt());
  }
  
  void spdifInCB(wxCommandEvent &event) override
  {
    card->spdifIn.set(event.GetInt());
  }
  
  void outputLevelCB(wxCommandEvent &event) override
  {
    card->outputLevel.set(event.GetInt());
  }
  
  void spdifOpticalCB(wxCommandEvent &event) override
  {
    card->spdifOpt.set(event.GetInt());
  }
  
  void spdifProfessionalCB(wxCommandEvent &event) override
  {
    card->spdifPro.set(event.GetInt());
  }
  
  void analogOutCB(wxCommandEvent &event) override
  {
    card->xlr.set(1 - event.GetInt());
  }
  
  void phonesLevelCB(wxCommandEvent &event) override
  {
    card->phonesLevel.set(event.GetInt());
  }
  
  void singleSpeedWclkOutCB(wxCommandEvent &event) override
  {
    card->singleSpeedWclkOut.set(event.GetInt());
  }
  
  void tmsCB(wxCommandEvent &event) override
  {
    card->clrTms.set(!event.GetInt());
  }

  void adatInternalCB(wxCommandEvent &event) override
  {
    card->adatInternal.set(event.GetInt());
  }
};

AioCard::AioCard(int index)
  : HDSPeCard(index)
  , inputLevel(this, "Input Level")
  , outputLevel(this, "Output Level")
  , phonesLevel(this, "Phones Level")
  , spdifIn(this, "S/PDIF In")
  , spdifOpt(this, "S/PDIF Out Optical")
  , spdifPro(this, "S/PDIF Out Professional")
  , singleSpeedWclkOut(this, "Single Speed WordClk Out")
  , clrTms(this, "Clear TMS")
  , xlr(this, "XLR Breakout Cable")
  , ai4s(this, "AI4S Present")
  , ao4s(this, "AO4S Present")
  , adatInternal(this, "ADAT Internal")
{
  modelName = "AIO";
  tcoSyncChoice = 4;
}

AioCard::~AioCard()
{
}

class wxPanel* AioCard::makePanel(wxWindow* parent)
{
  return panel = new MyAioPanel(this, parent);
}
