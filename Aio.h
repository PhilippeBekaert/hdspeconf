/*! \file Aio.h
 *! \brief RME HDSPe Aio panel and control.
 * 20211117,19 - Philippe.Bekaert@uhasselt.be */

#ifndef _AIO_H_
#define _AIO_H_

#include "HDSPeCard.h"

class AioCard: public HDSPeCard {
 protected:
  friend class MyAioPanel;
  class MyAioPanel* panel {nullptr};
  
  SndEnumControl inputLevel;
  SndEnumControl outputLevel;
  SndEnumControl phonesLevel;
  SndEnumControl spdifIn;
  SndBoolControl spdifOpt;
  SndBoolControl spdifPro;
  SndBoolControl singleSpeedWclkOut;
  SndBoolControl clrTms;
  SndBoolControl xlr;
  SndBoolControl ai4s;
  SndBoolControl ao4s;
  SndBoolControl adatInternal;

public:
  AioCard(int index);
  ~AioCard();

  class wxPanel* makePanel(class wxWindow* parent) override;
};

#endif /* _AIO_H_ */
