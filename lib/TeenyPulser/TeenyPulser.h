/*
TeenyPulser - a small library for pulsing LEDs and buzzers

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef TEENYPULSER_h
#define TEENYPULSER_h

#include "Arduino.h"

/********************************************************************/
// Declaration of TeenyPulser class
/********************************************************************/
class TeenyPulser
{

public:

  // ----- Constructor -----
  // @param 'pin_'                      -The pin to be pulsed.
  // @param 'activeHigh_'               -Set the active level - true=high false=low.
  // @param 'pulseWidth_'               -Sets active unit pulse width in milliseconds.
  // @param 'pulseInterval_'            -Sets gap between pulses in milliseconds.
  // @param 'repeatPeriod_'             -Sets period between repeating pulse trains.
  // @param 'longPulseWidthMultiplier_' -multiplies pulseWidth for long pulses
  TeenyPulser(int pin_, bool activeHigh_=true, uint32_t pulseWidth_=50, uint32_t pulseInterval_=350, uint32_t repeatPeriod_=4000, uint32_t longPulseWidthMultiplier_=4);

  void init();   // call once during setup

  void pulse(uint8_t pulseCount);
  void pulse(uint8_t pulseCount, uint32_t pulseWidthMultiplier);
  void pulse_repeat(uint8_t pulseCount);
  void pulse_repeat(uint8_t pulseCount, uint32_t pulseWidthMultiplier);
  void pulse_short(uint8_t pulseCount);
  void pulse_short_repeat(uint8_t pulseCount);
  void pulse_long(uint8_t pulseCount);
  void pulse_long_repeat(uint8_t pulseCount);
  void pulse_stop();
  void pulse_stop_repeat();

  // ----- State machine -----
  void tick();   // must be called periodically (~50ms when convenient)

private:

  int _pin;            // hardware pin number
  int _pulseOnLevel;   // active logic level
  int _pulseOffLevel;  // inactive logic level

  uint32_t  _pulseWidth;
  uint32_t  _pulseInterval;
  bool      _repeat;
  uint32_t  _repeatPeriod;
  uint32_t  _pulseWidthMultiplier;
  uint32_t  _longPulseWidthMultiplier;
  uint32_t  _pulseTimer;
  uint32_t  _repeatTimer;
  uint8_t   _pulseCount;
  int8_t    _pulseCountdown;
  bool      _updateInProgress;

  void pulse_exec(uint8_t pulseCount, uint32_t pulseWidthMultiplier, bool repeat);

  // pulse states
  enum stateMachine_t : int {
    PS_STOP = 0,
    PS_START = 1,
    PS_ON = 2,
    PS_OFF = 3,
    PS_IDLE = 4
  };
  stateMachine_t _pulseState = PS_IDLE;

};

#endif

