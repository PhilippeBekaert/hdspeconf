/*! \file AioPro.cpp
 *! \brief RME HDSPe Aio Pro panel and control.
 * 20210811,12,0908,09,10,11,16 - Philippe.Bekaert@uhasselt.be */

#include <thread>
#include <iostream>

#include "SndControl.h"
#include "AioPro.h"
#include "AioProPanel.h"

#include "HDSPeConf.h"

#define SET_CB(prop) card->prop.callOnValueChange(POSTCB(update_##prop,#prop))

class MyAioProPanel: public AioProPanel {
 protected:
  class AioProCard* card { nullptr };

  constexpr static const double UNSET_PITCH { -1.0 };
  double newPitch = UNSET_PITCH;
  
 public:
  MyAioProPanel(AioProCard* _card, wxWindow* parent)
    : AioProPanel(parent, wxID_ANY)
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
    
    SET_CB(inputLevel);
    SET_CB(outputLevel);
    SET_CB(phonesLevel);
    SET_CB(spdifIn);
    SET_CB(spdifOpt);
    SET_CB(spdifPro);
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
    // 3 - level because the buttons are declared bottom-to-top.
    inputLevelBox->SetSelection(3 - card->inputLevel);
  }

  void update_outputLevel(void)
  {
    int level = card->getOutputLevel();
    int xlr = card->outOnXlr();
    setOutputLevelLabels(xlr);
    outputLevelBox->SetSelection(3 - level);
    analogOutBox->SetSelection(1 - xlr);
  }

  void update_phonesLevel(void)
  {
    phonesLevelBox->SetSelection(1 - card->phonesLevel);
  }

  void update_spdifIn(void)
  {
    spdifInBox->SetSelection(2 - card->spdifIn);
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

  //! \brief Set output level radio box label texts depending on whether
  //! we output on RCA or XLR.
  void setOutputLevelLabels(bool xlr)
  {
    static const char* xlrTexts[4] = {
      "+24 dBu",
      "+19 dBu",
      "+13 dBu",
      "+4 dBu"
    };
    static const char* rcaTexts[4] = {
      "+19 dBu",
      "+13 dBu",
      "+4 dBu",
      "-2 dBu"
    };
    const char** texts = xlr ? xlrTexts : rcaTexts;
    for (int i=0; i<4; i++)
      outputLevelBox->SetString(i, texts[i]);
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
    card->inputLevel.set(3 - event.GetInt());
  }
  
  void spdifInCB(wxCommandEvent &event) override
  {
    card->spdifIn.set(2 - event.GetInt());
  }
  
  void outputLevelCB(wxCommandEvent &event) override
  {
    card->outputLevel.set((card->outOnXlr() ? 4 : 0)
				  + (3 - event.GetInt()));
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
    card->outputLevel.set(((1 - event.GetInt()) ? 4 : 0)
				  + card->getOutputLevel());
  }
  
  void phonesLevelCB(wxCommandEvent &event) override
  {
    card->phonesLevel.set(1 - event.GetInt());
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

AioProCard::AioProCard(int index)
  : HDSPeCard(index)
  , inputLevel(this, "Input Level")
  , outputLevel(this, "Output Level")
  , phonesLevel(this, "Phones Level")
  , spdifIn(this, "S/PDIF In")
  , spdifOpt(this, "S/PDIF Out Optical")
  , spdifPro(this, "S/PDIF Out Professional")
  , singleSpeedWclkOut(this, "Single Speed WordClk Out")
  , clrTms(this, "Clear TMS")
{
  modelName = "Aio PRO";
  tcoSyncChoice = 4;
}

AioProCard::~AioProCard()
{
}

class wxPanel* AioProCard::makePanel(wxWindow* parent)
{
  return panel = new MyAioProPanel(this, parent);
}

int AioProCard::outOnXlr(void) const
{
  return outputLevel / 4;
}

int AioProCard::getOutputLevel(void) const
{
  return outputLevel % 4;
}
