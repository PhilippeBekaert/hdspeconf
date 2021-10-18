/*! \file debug.h
 *! \brief Debugging and testing aids.
 * 20210812 - Philippe.Bekaert@uhasselt.be */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <chrono>

// returns system clock seconds elapsed since start_c.
static inline
double elapsed_seconds(const std::chrono::system_clock::time_point& start_c)
{
  auto now_c = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = now_c - start_c;
  return elapsed.count();
}

static inline
auto get_time(void)
{
  return std::chrono::system_clock::now();
}

#endif /*_DEBUG_H_*/
