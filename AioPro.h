/*! \file AioPro.h
 *! \brief RME HDSPe Aio Pro panel and control.
 * 20210811,12,0910 - Philippe.Bekaert@uhasselt.be */

#ifndef _AIO_PRO_H_
#define _AIO_PRO_H_

#include "HDSPeCard.h"

class AioProCard: public HDSPeCard {
 protected:
  friend class MyAioProPanel;
  class MyAioProPanel* panel {nullptr};
  
  SndEnumControl inputLevel;
  SndEnumControl outputLevel;
  SndEnumControl phonesLevel;
  SndEnumControl spdifIn;
  SndBoolControl spdifOpt;
  SndBoolControl spdifPro;
  SndBoolControl singleSpeedWclkOut;
  SndBoolControl clrTms;

  int outOnXlr(void) const;       // 1 if output on XLR, 0 if on RCA
  int getOutputLevel(void) const; // 0,1,2,3 - different meanings for XLR / RCA
  
 public:
  AioProCard(int index);
  ~AioProCard();

  class wxPanel* makePanel(class wxWindow* parent) override;
};

#endif /* _AIO_PRO_H_ */
