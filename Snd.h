/* \file Snd.h
 * \brief Alsa++ common stuff.
 * Philippe.Bekaert@uhasselt.be - 20210904 */

#pragma once

#include <alsa/asoundlib.h>

//! \brief Checks the return code of the ALSA routine snd_<what>().
//! Throws a std::runtime_error with appropriate message if negative.
//! Simply returns the error code if zero or positive.
static inline int SndCheckErr(int alsa_error, const char* what)
{
  if (alsa_error < 0)
    throw std::runtime_error("ALSA snd_" + std::string(what) + " error " + std::to_string(alsa_error) + ": " + std::string(snd_strerror(alsa_error)));
  return alsa_error;
}
