/*! \file TCO.h
 *! \brief RME HDSP Time Code Option module status and control.
 * Philippe.Bekaert@uhasselt.be - 20210813,0911,20220330 */

#ifndef _TCO_H_
#define _TCO_H_

#include "TCOPanel.h"
#include "SndControl.h"

class MyTCOPanel: public TCOPanel {
 public:
  MyTCOPanel(class HDSPeTCO* tco, class wxWindow* parent);
  
 protected:
  class HDSPeTCO* tco { nullptr };

  void update_ltcIn(void);
  void update_ltcInValid(void);
  void update_ltcInFps(void);
  void update_ltcInDropFrame(void);
  void update_ltcInPullFac(void);
  void update_videoFormat(void);
  void update_videoFps(void);
  void update_wckValid(void);
  void update_wckSpeed(void);
  void update_lock(void);
  void update_sampleRate(void);
  void update_pull(void);
  void update_wckConversion(void);
  void update_frameRate(void);
  void update_syncSrc(void);
  void update_wordTerm(void);
  void update_ltcOut(void);
  void update_ltcRun(void);
  void update_ltcJamSync(void);

  void ltcSyncCB(wxCommandEvent &event) override;
  void videoSyncCB(wxCommandEvent &event) override;
  void wckSyncCB(wxCommandEvent &event) override;
  void termCB(wxCommandEvent &event) override;
  void ltcFrameRateCB(wxCommandEvent &event) override;
  void dropFrameCB(wxCommandEvent &event) override;
  void wckConversionCB(wxCommandEvent &event) override;
  void ltcSampleRateCB(wxCommandEvent &event) override;
  void pullCB(wxCommandEvent &event) override;
  void useTcoCB(wxCommandEvent &event) override;
  void autoCB(wxCommandEvent &event) override;
  void ltcRunCB(wxCommandEvent &event) override;
  void positionalCB(wxCommandEvent &event) override;
  void wallClockCB(wxCommandEvent &event) override;
  void jamSyncCB(wxCommandEvent &event) override;

#ifdef NEVER  
  void setPullLabels(void);
#endif /*NEVER*/
  void setFrameRateLabels(bool df);
  void setLtcIn(void);
  void setLtcInFrameRate(void);
  void setWckStatus(void);

  void update_preferredRef(void);
  void update_systemSampleRate(void);
  void setCardStatus(void);
  SndControl::Callback update_cardSampleRate { nullptr };
  SndControl::Callback update_cardPreferredRef { nullptr };
};

#endif /* _TCO_H_ */
