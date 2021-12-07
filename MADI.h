/*! \file MADI.h
 *! \brief RME HDSPe MADI panel and control.
 * 20211207 - Philippe.Bekaert@uhasselt.be */

#ifndef _MADI_H_
#define _MADI_H_

#include "HDSPeCard.h"

class MADICard: public HDSPeCard {
 protected:
  friend class MyMADIPanel;
  class MyMADIPanel* panel {nullptr};

  SndEnumControl externalFreq;
  SndEnumControl preferredInput;
  SndEnumControl currentInput;
  SndBoolControl autoselectInput;
  SndBoolControl rx64ch;
  SndBoolControl tx64ch;
  SndBoolControl doubleWire;
  SndBoolControl singleSpeedWclkOut;
  SndBoolControl clrTms;
  
 public:
  MADICard(int index);
  ~MADICard();

  class wxPanel* makePanel(class wxWindow* parent) override;
};

#endif /* _MADI_H_ */
