/*! \file HDSPeConf.h
 *! \brief Global functions. 
 * Philippe.Bekaert@uhasselt.be - 20210907,15,16 */

#pragma once

#include <sys/types.h>

//! \brief Post a callback function.
extern void PostCB(std::function<void(void)> cb);

#define POSTCB(cb,prop) [this](){ PostCB([this](){ cb(); }); }

