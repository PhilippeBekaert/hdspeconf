/*! \file RayDAT.h
 *! \brief RME HDSPe RayDAT panel and control.
 * 20211120 - Philippe.Bekaert@uhasselt.be */

#ifndef _RAYDAT_H_
#define _RAYDAT_H_

#include "HDSPeCard.h"

class RayDATCard: public HDSPeCard {
 protected:
  friend class MyRayDATPanel;
  class MyRayDATPanel* panel {nullptr};
  
  SndEnumControl spdifIn;
  SndBoolControl spdifOpt;
  SndBoolControl spdifPro;
  SndBoolControl singleSpeedWclkOut;
  SndBoolControl clrTms;
  SndBoolControl adat1Internal;
  SndBoolControl adat2Internal;  
  
 public:
  RayDATCard(int index);
  ~RayDATCard();

  class wxPanel* makePanel(class wxWindow* parent) override;
};

#endif /* _RAYDAT_H_ */
