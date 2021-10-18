/*! \file SndCard.h
 * \brief ALSA sound card control handle C++ wrapper.
 * \author Philippe Bekaert <Philippe.Bekaert@uhasselt.be>
 * \date 20210812, 0902,04,06,16
 *
 * See \ref cardplusplus for more details.
 */
/*! \page cardplusplus SndCard: ALSA sound card control handle C++ wrapper.
 *
 * Opening an ALSA sound card control interface with SndCard is a first step
 * before controls themselves can be accessed as explained in \ref controlplusplus.
 *
 * Construction
 * ------------
 *
 * Construct SndCard object from an ALSA sound card index or name.
 *
 * Control elements are pre-loaded and a asynchronous callback function
 * installed in order to catch element events.
 *
 * Getting information
 * -------------------
 *
 * - snd_ctl_t* and snd_hctl_t* handle are available via a cast operator,
 * so SndCard objects can be used in any ALSA function expecting a
 * snd_ctl_t* or snd_hctl_t* argument.
 * - SndCard::getInfo() returns a snd_ctl_card_info_t card information struct pointer.
 * - SndCard::getName(), SndCard::getCardIndex(), SndCard::getComponents(), SndCard::getDriver(), SndCard::getId(),
 * SndCard::getLongName() and SndCard::getMixerName() are short cuts retrieving from the
 * card info what the function name suggests.
 * - SndCard::getControlCount() returns the number of control elements loaded.
 * - SndCard::getControls() returns an std::vector<SndControl*> with all currently 
 * loaded control elements. SndControl items on the list need to be delete'd
 * by the caller when no longer needed.
 * Like this:
 *     
 *      SndCard* card = new SndCard("hw:2");
 *      ...
 *      const std::vector<class SndControl*> controls(std::move(card->getControls()));
 *      ... // do something with the controls 
 *      for (auto c : controls)
 *        delete c;
 *     
 * 
 * Other operations
 * ----------------
 * 
 * - SndCard::ioctl() performs a hwdep ioctl read or write on the sound card.
 */

#pragma once

#include <string>
#include <vector>

#include "Snd.h"

//! \brief ALSA sound card control handle C++ wrapper.
//!
//! See \ref cardplusplus page.
class SndCard {
protected:
  friend class SndControl;

  std::string name;                   //!< ALSA card name.
  
  snd_ctl_t *ctl { nullptr };         //!< ALSA sound card control handle.
  snd_hctl_t *hctl { nullptr };       //!< ALSA sound card hcontrol handle.
  snd_ctl_card_info_t *card_info { nullptr };   //!< ALSA sound card info.

  //! \brief Open the sound card control handle, preload control elements
  //! and install an asynchronous callback handler to catch element
  //! change notification events.
  void open(const std::string& cardName);

  //! \brief Close the sound card control handle.
  void close(void);

public:
  //! \brief Constructor: open sound card by index.
  SndCard(int index)
    : name("hw:" + std::to_string(index))
  {
    open(name);
  }

  //! \brief Constructor: open sound card by name.
  SndCard(const std::string& cardName)
    : name(cardName)
  {
    open(name);
  }

  //! \brief Destructor: close the sound card control handle.
  virtual ~SndCard()
  {
    close();
  }

  //! \brief Get control handle for the card. E.g. use a SndCard object
  //! as argument for any ALSA function that takes a snd_ctl_t* argument.
  operator snd_ctl_t*() const { return ctl; }

  //! \brief Get high-level control handle for the card. E.g. use a SndCard
  //! object as argument for any ALSA functoin that takes a snd_hctl_t*
  //! argument.
  operator snd_hctl_t*() const { return hctl; }

  //! \brief Return card info.
  snd_ctl_card_info_t* getInfo(void) const { return card_info; }

  //! \brief Get the ALSA sound card name.
  const std::string getName(void) const
  {
    return std::string(snd_hctl_name(hctl));
  }

  //! \brief Get ALSA sound card index number.
  int getCardIndex(void) const
  {
    return snd_ctl_card_info_get_card(card_info);
  }

  //! \brief Get ALSA sound card components.
  const std::string getComponents(void) const
  {
    return std::string(snd_ctl_card_info_get_components(card_info));
  }

  //! \brief Get ALSA sound card driver name.
  const std::string getDriver(void) const
  {
    return std::string(snd_ctl_card_info_get_driver(card_info));
  }

  //! \brief Get ALSA sound card Id.
  const std::string getId(void) const
  {
    return std::string(snd_ctl_card_info_get_id(card_info));
  }

  //! \brief Get ALSA sound card long name.
  const std::string getLongName(void) const
  {
    return std::string(snd_ctl_card_info_get_longname(card_info));
  }

  //! \brief Get ALSA sound card on-board mixer name.
  const std::string getMixerName(void) const
  {
    return std::string(snd_ctl_card_info_get_mixername(card_info));
  }

  //! \brief Return the number of control elements on the card.
  unsigned getControlCount(void) const
  {
    return snd_hctl_get_count(hctl);
  }

  //! \brief List all controls on the card.
  //! \note Use std::move() to avoid copying the returned std::vector.
  //! \note Elements eventually must be deleted by the caller.
  const std::vector<class SndControl*> getControls(void) const;

  //! \brief Perform a hwdep ioctl on the sound card.
  void ioctl(uint32_t request, int mode, void* pdata) const;

protected:
  class SndCardEventThread* evThread { nullptr }; //!< Event handling thread.
};
