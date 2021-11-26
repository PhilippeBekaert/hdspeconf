/*! \file AES.h
 *! \brief RME HDSPe AES panel and control.
 * 20211120 - Philippe.Bekaert@uhasselt.be */

#ifndef _AES_H_
#define _AES_H_

#include "HDSPeCard.h"

class AESCard: public HDSPeCard {
 protected:
  friend class MyAESPanel;
  class MyAESPanel* panel {nullptr};
  
  SndEnumControl doubleSpeedMode;
  SndEnumControl quadSpeedMode;  
  SndBoolControl professional;
  SndBoolControl emphasis;
  SndBoolControl nonAudio;
  SndBoolControl singleSpeedWclkOut;
  SndBoolControl clrTms;
  
 public:
  AESCard(int index);
  ~AESCard();

  class wxPanel* makePanel(class wxWindow* parent) override;
};

#endif /* _AES_H_ */
