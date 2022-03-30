/*! \file TCO.cpp
 *! \brief RME HDSP Time Code Option module status and control.
 * Philippe.Bekaert@uhasselt.be - 20210813,14,0902,07,11,16,28,29,1009,20220321,30 */

#include <stdio.h>
#include <time.h>

#include "TCO.h"
#include "HDSPeCard.h"
#include "SndControl.h"

#include "HDSPeConf.h"

#ifdef DEBUG
static std::ostream& LtcPrint(std::ostream& s, const std::vector<long long>& ltc, bool df)
{
  static uint64_t prevFrameCount {0};
  uint64_t frameCount = ltc[1];
  char buf[100];
  snprintf(buf, sizeof(buf), "%02llu:%02llu:%02llu%c%02llu @ %ld +%ld",
	   ((ltc[0] >> 56) & 0x03)*10 + ((ltc[0] >> 48) & 0x0f),
	   ((ltc[0] >> 40) & 0x07)*10 + ((ltc[0] >> 32) & 0x0f),
	   ((ltc[0] >> 24) & 0x07)*10 + ((ltc[0] >> 16) & 0x0f),
	   df ? '.' : ':',
	   ((ltc[0] >>  8) & 0x03)*10 + ((ltc[0] >>  0) & 0x0f),
	   frameCount, frameCount - prevFrameCount);
  prevFrameCount = frameCount;
  return s << buf;
}
#endif /*DEBUG*/

//////////////////////////////////////////////////////////////////////////////

#define SET_CB(prop) tco->prop.callOnValueChange(POSTCB(update_##prop,#prop))

MyTCOPanel::MyTCOPanel(class HDSPeTCO* _tco, class wxWindow* parent)
  : TCOPanel(parent, wxID_ANY)
  , tco(_tco)
{
#ifdef NEVER  
  setPullLabels();
#endif /*NEVER*/
  
  // Need to add our own callback to the card panel callbacks for
  // sampleRate and preferredRef.
  update_cardPreferredRef = tco->card->preferredRef.callOnValueChange
    ([this](){PostCB([this](){update_preferredRef();});});
  update_cardSampleRate = tco->card->sampleRate.callOnValueChange
    ([this](){PostCB([this](){update_systemSampleRate();});});
  if (!update_cardPreferredRef || !update_cardSampleRate)
    throw std::runtime_error("MyTCOPanel: card callbacks not set!\n");
  
  SET_CB(ltcIn);
  SET_CB(ltcInValid);
  SET_CB(ltcInFps);
  SET_CB(ltcInDropFrame);
  SET_CB(ltcInPullFac);
  SET_CB(videoFormat);
  SET_CB(videoFps);
  SET_CB(wckValid);
  SET_CB(wckSpeed);
  SET_CB(lock);
  
  SET_CB(sampleRate);
  SET_CB(pull);
  SET_CB(wckConversion);
  SET_CB(frameRate);
  SET_CB(syncSrc);
  SET_CB(wordTerm);

  SET_CB(ltcOut);
  SET_CB(ltcRun);

  ltcSyncButton->Enable(tco->firmware < 11);  // LTC sync is not reliable and no longer available when firmware version is 11 or later.
  videoSyncButton->Enable(true); // always enable: video format is only detected when this button is selected.
  wckSyncButton->Enable(true);   // always enable: word clock speed is only detected when this button is selected.
}

void MyTCOPanel::setLtcIn(void)
{
  unsigned long long ltc = tco->ltcIn;
  char buf[20];
  snprintf(buf, sizeof(buf), "%02llu:%02llu:%02llu%c%02llu",
	   ((ltc >> 56) & 0x03)*10 + ((ltc >> 48) & 0x0f),
	   ((ltc >> 40) & 0x07)*10 + ((ltc >> 32) & 0x0f),
	   ((ltc >> 24) & 0x07)*10 + ((ltc >> 16) & 0x0f),
	   ':',
	   ((ltc >>  8) & 0x03)*10 + ((ltc >>  0) & 0x0f));
  ltcStatusLabel->SetLabel(tco->ltcInValid ? buf : "--:--:--:--");
}

void MyTCOPanel::setLtcInFrameRate(void)
{
  const char* fps[4] = { "24", "25", "29.97", "30" };
  const char* df[2] = { "fps", "dfps" };
  char text[25];
  int pullfac = tco->ltcInPullFac - 1000;

  if (pullfac != 0)
    snprintf(text, sizeof(text), "%s %s        %+.1f %%",
	     fps[tco->ltcInFps], df[tco->ltcInDropFrame],
	     (double)pullfac * 0.1);
  else
    snprintf(text, sizeof(text), "%s %s", 
	     fps[tco->ltcInFps], df[tco->ltcInDropFrame]);
  
  ltcInFrameRateLabel->SetLabel(tco->ltcInValid ? text : "");
}

void MyTCOPanel::update_ltcIn(void)
{
  setLtcIn();
#ifdef DEBUG  
  LtcPrint(std::cerr, tco->ltcIn, false) << "\n";
#endif /*DEBUG*/
}

void MyTCOPanel::update_ltcInValid(void)
{
  setLtcIn();
  setLtcInFrameRate();
}

void MyTCOPanel::update_ltcInFps(void)
{
  setLtcInFrameRate();
}

void MyTCOPanel::update_ltcInDropFrame(void)
{
  setLtcInFrameRate();
}

void MyTCOPanel::update_ltcInPullFac(void)
{
  setLtcInFrameRate();
}

void MyTCOPanel::update_videoFormat(void)
{
  //  std::cerr << __func__ << ":" << __LINE__ << ": videoFormat=" << tco->videoFormat << "\n";
  if (tco->firmware < 11)
    videoStatusLabel->SetLabel(tco->videoFormat==0 ? std::string("") : tco->videoFormat.label());
}

void MyTCOPanel::update_videoFps(void)
{
  //  std::cerr << __func__ << ":" << __LINE__ << ": videoFps=" << tco->videoFps << "\n";
  if (tco->firmware >= 11)
    videoStatusLabel->SetLabel(tco->videoFps==0 ? std::string("No Video") :
			       (std::string(tco->videoFps.label()) + " fps"));
}

void MyTCOPanel::setWckStatus(void)
{
  wckStatusLabel->SetLabel(!tco->wckValid ? std::string("") : tco->wckSpeed.label());
}

void MyTCOPanel::update_wckValid(void)
{
  setWckStatus();
}

void MyTCOPanel::update_wckSpeed(void)
{
  setWckStatus();
}

void MyTCOPanel::update_lock(void)
{
  lockLabel->SetLabel(tco->lock ? "TCO Lock" : "No TCO Lock");
  lockLabel->SetBackgroundColour(tco->lock
				 ? wxNullColour : wxColour(0xff, 0xc6, 0x00));
}

void MyTCOPanel::update_sampleRate(void)
{
  ltcSampleRateBox->SetSelection(tco->sampleRate);
}

void MyTCOPanel::update_pull(void)
{
  pullBox->SetSelection(tco->pull);
}

void MyTCOPanel::update_wckConversion(void)
{
  wckConversionBox->SetSelection(tco->wckConversion);
}

void MyTCOPanel::update_frameRate(void)
{
  int fps, df;
  tco->getFrameRate(&fps, &df);
  
  ltcFrameRateBox->SetSelection(fps);

  setFrameRateLabels(df);  
  dropFrameButton->SetValue(df);
  dropFrameButton->Enable(fps == 2 || fps == 3);
}

void MyTCOPanel::update_syncSrc(void)
{
  wckSyncButton->SetValue(tco->syncSrc == 0);
  videoSyncButton->SetValue(tco->syncSrc == 1);
  ltcSyncButton->SetValue(tco->syncSrc == 2);
}

void MyTCOPanel::update_wordTerm(void)
{
  termButton->SetValue(tco->wordTerm);
}

void MyTCOPanel::update_preferredRef(void)
{
  update_cardPreferredRef();
  setCardStatus();
}

void MyTCOPanel::update_systemSampleRate(void)
{
  update_cardSampleRate();
  setCardStatus();
}

void MyTCOPanel::update_ltcOut(void)
{
}

void MyTCOPanel::update_ltcRun(void)
{
  ltcRunButton->SetValue(tco->ltcRun);
}

void MyTCOPanel::setCardStatus(void)
{
  useTCOButton->SetValue(tco->card->isSyncedToTco());

  char buf[20];
  double sampleRate = tco->card->getSystemSampleRate();
  snprintf(buf, sizeof(buf), "%.1f", sampleRate);
  sampleRateLabel->SetLabelText(buf);
  sampleRateLabel->SetBackgroundColour(tco->card->isStandardSampleRate(sampleRate)
				       ? wxNullColour : wxColour(0xff, 0xc6, 0x00));
}

#ifdef NEVER
// We need this function because wxGlade C++ code generation crashes when
// there are % characters in the label texts. (Problem fixed in newer versions
// of wxGlade).
void MyTCOPanel::setPullLabels(void)
{
    static const char* texts[5] = {
      "None",
      "+0.1 % ",
      "- 0.1 %",
      "+4 % ",
      "- 4 %"
    };
    for (int i=0; i<5; i++)
      pullBox->SetString(i, texts[i]);    
}
#endif /*NEVER*/

void MyTCOPanel::setFrameRateLabels(bool df)
{
    static const char* dfTexts[4] = {
      "24 fps",
      "25 fps",
      "29.97 dfps",
      "30 dfps"
    };
    static const char* ffTexts[4] = {
      "24 fps",
      "25 fps",
      "29.97 fps",
      "30 fps"
    };
    const char** texts = df ? dfTexts : ffTexts;
    for (int i=0; i<4; i++)
      ltcFrameRateBox->SetString(i, texts[i]);  
}

void MyTCOPanel::ltcSyncCB(wxCommandEvent &event)
{
  tco->syncSrc.set(2);
}

void MyTCOPanel::videoSyncCB(wxCommandEvent &event)
{
  tco->syncSrc.set(1);  
}

void MyTCOPanel::wckSyncCB(wxCommandEvent &event)
{
  tco->syncSrc.set(0);    
}

void MyTCOPanel::termCB(wxCommandEvent &event)
{
  tco->wordTerm.set(event.GetInt());
}

void MyTCOPanel::ltcFrameRateCB(wxCommandEvent &event)
{
  int fps, df;
  tco->getFrameRate(&fps, &df);
  tco->setFrameRate(event.GetInt(), df);
}

void MyTCOPanel::dropFrameCB(wxCommandEvent &event)
{
  int fps, df;
  tco->getFrameRate(&fps, &df);
  tco->setFrameRate(fps, event.GetInt());

  /* proposed change may have been refused */
  tco->getFrameRate(&fps, &df);
  dropFrameButton->SetValue(df);
  dropFrameButton->Enable(fps == 2 || fps == 3);
}

void MyTCOPanel::wckConversionCB(wxCommandEvent &event)
{
  tco->wckConversion.set(event.GetInt());
}

void MyTCOPanel::ltcSampleRateCB(wxCommandEvent &event)
{
  tco->sampleRate.set(event.GetInt());
}

void MyTCOPanel::pullCB(wxCommandEvent &event)
{
  tco->pull.set(event.GetInt());
}

void MyTCOPanel::useTcoCB(wxCommandEvent &event)
{
  tco->card->syncToTco(event.GetInt());
}

void MyTCOPanel::autoCB(wxCommandEvent &event)
{
  if (!tco->ltcInValid) {
    std::cerr << "No valid LTC input to jam sync with!\n";
    return;
  }

  int fps = tco->ltcInFps;
  int df = tco->ltcInDropFrame;
  if (df)
    fps = 2;      // 29.97 fps

  int pull = 0;
  if (tco->ltcInPullFac == 999) {
    if (fps == 3)
      fps = 2;    // 30 - 0.1% = 29.97
    else if (fps != 2)
      pull = 1;   // -0.1%
  }
  
  tco->setFrameRate(fps, df);
  tco->pull.set(pull);
  tco->sampleRate.set(2);   // "From App"
}

void MyTCOPanel::ltcRunCB(wxCommandEvent &event)
{
  tco->ltcRun.set(event.GetInt());
}

void MyTCOPanel::positionalCB(wxCommandEvent &event)
{
  // time code 00:00:00:00 at frame count 0 yields positional time code.
  std::vector<long long> ltc { 0, 0 };
  tco->ltcOut.set(ltc);
}

void MyTCOPanel::wallClockCB(wxCommandEvent &event)
{
  // The special time code 0x030f070f070f030f means 'real time'.
  // The frame count argument is an offset in seconds to be added to the
  // systems real time clock reading, and is used to correct for timezone
  // and daylight saving time. The code below assumes the RTC is in UTC
  // and not in local time zone. Check/correct with timedatectl command.
  time_t t;
  time(&t);
  struct tm tm;
  localtime_r(&t, &tm);
  std::vector<long long> ltc { 0x030f070f070f030f, tm.tm_gmtoff };
  tco->ltcOut.set(ltc);
}

void MyTCOPanel::jamSyncCB(wxCommandEvent &event)
{
  if (!tco->ltcInValid) {
    std::cerr << "No valid LTC input to jam sync with!\n";
    return;
  }

  const std::vector<long long> ltc = tco->ltcIn;
  tco->ltcOut.set(ltc);
}

