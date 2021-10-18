/*! \file SndCard.cpp
 *! \brief ALSA sound card handle C++ wrapper.
 * 20210812,0902,06,16 - Philippe.Bekaert@uhasselt.be */

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <atomic>

#include <alsa/asoundlib.h>

#include "SndCard.h"
#include "SndControl.h"

//! \brief SndCard control event handling thread.
class SndCardEventThread {
public:
  SndCardEventThread(SndCard* _card)
    : card(_card)
    , t([this](){main();})
  {
  }

  ~SndCardEventThread()
  {
    shutdown_requested = true;
    t.join();
  }

private:
  SndCard* card { nullptr };
  std::thread t;
  std::atomic<bool> shutdown_requested {false};
  
  void main(void)
  {
    try {
      while (!shutdown_requested) {
      if (snd_hctl_wait(*card, 1000))
	SndCheckErr(snd_hctl_handle_events(*card), "hctl_handle_events");
    }
    } catch (std::runtime_error& e) {
      std::cerr << "SndCardEventThread: " << e.what() << "on card " << card->getName() << "\n";
    } catch (...) {
      std::cerr << "SndCardEventThread: unknown error.\n";
    }
  }  
};

void SndCard::open(const std::string& name)
{
  // open the card.
  SndCheckErr(snd_hctl_open(&hctl, name.c_str(), SND_CTL_NONBLOCK), "hctl_open_ctl");
  ctl = snd_hctl_ctl(hctl);

  // load card info.
  snd_ctl_card_info_malloc(&card_info);
  SndCheckErr(snd_ctl_card_info(ctl, card_info), "ctl_card_info");
  
  // pre-load control elements.
  SndCheckErr(snd_hctl_load(hctl), "hctl_load");

  evThread = new SndCardEventThread(this);
}

void SndCard::close(void)
{
  delete evThread;
  
  snd_ctl_card_info_free(card_info);
  snd_hctl_close(hctl);
}

void SndCard::ioctl(uint32_t request, int mode, void* pdata) const
{
  snd_hwdep_t *hw {nullptr};
  SndCheckErr(snd_hwdep_open(&hw, name.c_str(), mode), "hwdep_open");
  SndCheckErr(snd_hwdep_ioctl(hw, request, pdata), "hwdep_ioctl");
  snd_hwdep_close(hw);
}

const std::vector<class SndControl*> SndCard::getControls(void) const
{
  std::vector<SndControl*> controls;

  for (snd_hctl_elem_t* elem = snd_hctl_first_elem(hctl);
       elem; elem = snd_hctl_elem_next(elem)) {
    controls.push_back(SndControl::Create(this, elem));
  }

  return controls;
}
