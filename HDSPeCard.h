/*! \file HDSPeCard.h
 *! \brief RME HDSPe sound card enumeration and common control.
 * 20210810,11,12,13,0906,08,09,10,11 - Philippe.Bekaert@uhasselt.be */

#ifndef _HDSPE_CARD_H_
#define _HDSPE_CARD_H_

#include <functional>
#include <ostream>
#include <vector>

#include "SndCard.h"
#include "SndControl.h"

//! \brief Enumerates RME HDSPe cards on the system at construction time.
//! Get the number of cards and pointers to the HDSPeCard object representing
//! each card after.
class HDSPeCardEnumerator {
 protected:
  std::vector<class HDSPeCard*> cards; //!< List of HDSPe cards on system.
  
 public:
  //! \brief Constructor: enumerated HDSPe driven cards on system.
  HDSPeCardEnumerator();

  //! \brief Destructor.
  ~HDSPeCardEnumerator();

  //! \brief Get the number of HDSPe cards on the system.
  int getCount(void) { return cards.size(); }

  //! \brief Get the i-th HDSPe card on the system.
  HDSPeCard* getCard(int i)
  {
    return i<0 || i>(int)cards.size() ? nullptr : cards[i];
  }

  //! \brief Get all HDSPe cards on the system.
  std::vector<class HDSPeCard*>& getCards(void) { return cards; }
};

//! \brief RME HDSPe sound card representation.
class HDSPeCard: public SndCard {
 protected:
  std::string modelName;  //!< Card model name
  int tcoSyncChoice {-1}; //!< preferred sync choice for TCO, -1 if no TCO
  
 public:
  //! \brief Constructor. Index is ALSA sound card index of the card to
  //! be opened. Properties are read during construction. Status polling
  //! is enabled.
  HDSPeCard(int index);

  //! \brief Destructor.
  virtual ~HDSPeCard();

  //! \brief Return a pretty name for the card, used as title for the
  //! settings panel.
  const std::string getPrettyName(void) const;

  //! \brief Create a settings panel for the card.
  virtual class wxPanel* makePanel(class wxWindow* parent) =0;

  //! \brief Returns true is card has a TCO module connected.
  bool hasTco(void) const;

  //! \brief Create TCO settings panel for card.
  class wxPanel* makeTcoPanel(class wxWindow* parent);

  //! \brief Synchronise card with TCO if enable is true. Sets master mode
  //! if false.
  void syncToTco(int enable);

  //! \brief Returns whether or not card is sync'ed to the TCO module.
  bool isSyncedToTco(void);
  
 public:
  //! \brief PCM running? If so, certain properties cannot be changed.
  bool isRunning(void) const;

  //! \brief Card in master clock mode?
  bool isMaster(void) const;

  //! \brief Return the effective current card sample rate, to sub-Hz
  //! accuracy (measured w.r.t. the cards own clock).
  double getSystemSampleRate(void) const;

  //! \brief Return internal sample rate (combination of frequency
  //! class and internal pich).
  double getInternalSampleRate(void) const;

  //! \brief Get the sample rate to compare clock sources with.
  //! This is the sample rate corresponding to the internal frequency
  //! class if running in master mode or there is no valid external
  //! source to sync with. Otherwise, it is the sample rate corresponding
  //! to the frequency class of the current external sync source.
  int getReferenceSampleRate(void) const;

  //! \brief Get the external frequency class, that is: the frequency class
  //! of the current autosync source. MADI cards have their own implementation.
  virtual int getExternalFreq(void) const;
  
  //! \brief Check whether a clock with frequency class <freq> is compatible
  //! with the reference sample rate. They are compatible if
  //! the single speed reduced freqRate(freq) and system sample rate
  //! do not deviate by more than 100 PPM.
  bool isClockCompatible(unsigned freq) const;

  //! \brief Convert frequency class <freq> to a frame rate:
  //! 1=32KHz, 2=44.1KHz, 3=48KHz, 4=64KHz, 5=88.2KHz, 6=96KHz, 7=128KHz,
  //! 8=176.4KHz, 9=192KHz. Other values for <freq> are invalid, and this
  //! function will return 0 for it.
  static int freqRate(unsigned freq);  
  
  //! \brief Returns the nearest standard sample rate. Standard sample rates
  //! are: 32000, 44100, 48000, 64000, 88200, 96000, 128000, 176400, 192000.
  static int nearestStandardSampleRate(int rate);

  //! \brief Returns true if rate deviates less than 100 PPM from the nearest
  //! standard sample rate.
  static bool isStandardSampleRate(double rate);

  // !\brief Convert double/quad speed rate to single speed rate.
  static double singleSpeedRate(double rate);

  //! \brief Returns true if rate deviates from ref by 100 PPM or more.
  static bool sampleRateDeviates(double rate, double ref);

  //! \brief Returns true if the single speed rate deviates from the
  //! single speed ref rate by 100 PPM or more.
  static bool singleSpeedRateDeviates(double rate, double ref);

  //! \brief Returns true if the internal sample rate deviates from the
  //! system sample rate by 100 PPM or more.
  bool internalRateDeviates(void) const;

  //! \brief Calculate pitch from frame rate: 0 if both rates are equal,
  //! <0 if rate<ref, >0 if rate>ref, -1 if rate==0.
  static double getPitch(double rate, double ref);
  double getPitch(double rate) const;

  //! \brief Get system pitch.
  double getPitch(void) const;

  // \brief Set internal pitch:
  void setPitch(double pitch);

  //! \brief Up 1 Hz
  double upPitch(void);

  //! \brief Down 1 Hz
  double downPitch(void);

  //! \brief -4% -> -0.1% -> 0.0% -> 0.1% -> 4%
  double nextPitch(void);

  //! \brief 4% -> 0.1% -> 0.0% -> -0.1% -> -4%
  double prevPitch(void);    

 protected:
  //! \brief Checks whether driver really is HDSPe, before initializing
  //! card properties during HDSPeCard construction.
  class DriverCheck {
  public:
    DriverCheck(SndCard* card);
  } driverCheck;
    
  // Status polling by the driver: the driver checks for status changes
  // <statusPolling> times per second. Each time a status change is detected,
  // or 2 seconds after the last change, the driver set <statusPolling> to 0,
  // effectively disabling it and causing our <onStatusChange> callback
  // to be invoked, as well as any registered callback of card status
  // properties that did change. The latter cause panel display updates on
  // their turn. Our <onStatusChange> callback re-enables status polling
  // whenever the driver disabled it, for as long as this program runs.
  SndIntControl statusPolling;
  void onStatusChange(void);
  static const int statusPollFreq { 10 };  // driver status poll frequency  
  
public:
  // HDSPe card info
  SndIntControl cardRevision;
  SndIntControl fwBuild;
  SndIntControl serial;
  SndBoolControl running;
  SndIntControl bufferSize;
  SndBoolControl tcoPresent;

  // Common HDSPe card status and controls
  SndEnumControl clockMode;
  SndEnumControl internalFreq;
  SndEnumControl preferredRef;
  SndEnumControl syncRef;
  SndEnumControl syncStatus;
  SndEnumControl syncFreq;
  SndInt64Control sampleRate; //!< system sample rate as a ratio.
  SndIntControl dds;          //!< raw internal pitch control, denominator
                              //! of ratio with same numerator as sampleRate.

  class HDSPeTCO* tco { nullptr };
};

//! \brief TCO module status and controls.
class HDSPeTCO {
public:
  HDSPeCard *card { nullptr };   //!< HDSPe card to which this TCO is connected.

  SndInt64Control ltcIn;
  SndBoolControl ltcInValid;
  SndEnumControl ltcInFps;
  SndBoolControl ltcInDropFrame;
  SndIntControl  ltcInPullFac;
  SndInt64Control ltcOut;
  SndBoolControl ltcRun;
  SndEnumControl sampleRate;
  SndEnumControl frameRate;
  SndEnumControl videoFormat;
  SndBoolControl wckValid;
  SndEnumControl wckSpeed;
  SndBoolControl lock;
  SndEnumControl pull;
  SndEnumControl wckConversion;
  SndEnumControl syncSrc;
  SndBoolControl wordTerm;
  
  //! \brief Constructor: loads properties for the TCO module on the card.
  HDSPeTCO(HDSPeCard* card);

  //! \brief Destructor.
  ~HDSPeTCO();

  //! \brief Set LTC out frame rate. frameRate property combines frames
  //! per second and drop frame format in one.
  void setFrameRate(int fps, int df);

  //! \brief Get current LTC out fps and drop frame flag from frameRate
  //! property.
  void getFrameRate(int* fps, int *df);
};

#endif /* _HDSPE_CARD_H_ */
