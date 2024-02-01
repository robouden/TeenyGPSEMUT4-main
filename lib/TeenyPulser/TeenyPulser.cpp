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

#include "TeenyPulser.h"

// ----- Constructor -----

TeenyPulser::TeenyPulser(int pin_, bool activeHigh_, uint32_t pulseWidth_, uint32_t pulseInterval_, uint32_t repeatPeriod_, uint32_t longPulseWidthMultiplier_) :
  _pin(pin_),
  _pulseWidth(pulseWidth_),
  _pulseInterval(pulseInterval_),
  _repeatPeriod(repeatPeriod_),
  _longPulseWidthMultiplier(longPulseWidthMultiplier_)
{
  if (activeHigh_) {
    _pulseOnLevel = HIGH;
    _pulseOffLevel = LOW;
  } else {
    _pulseOnLevel = LOW;
    _pulseOffLevel = HIGH;
  }
}

void TeenyPulser::init() {
  _pulseState = TeenyPulser::PS_STOP;
  _pulseCount = 0;
  _repeat = false;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, _pulseOffLevel);
}

void TeenyPulser::pulse(uint8_t pulseCount) {
  pulse_exec(pulseCount, 1, false);
}

void TeenyPulser::pulse_repeat(uint8_t pulseCount) {
  pulse_exec(pulseCount, 1, true);
}

void TeenyPulser::pulse(uint8_t pulseCount, uint32_t pulseWidthMultiplier) {
  pulse_exec(pulseCount, pulseWidthMultiplier, false);
}

void TeenyPulser::pulse_repeat(uint8_t pulseCount, uint32_t pulseWidthMultiplier) {
  pulse_exec(pulseCount, pulseWidthMultiplier, true);
}

void TeenyPulser::pulse_short(uint8_t pulseCount) {
  pulse_exec(pulseCount, 1, false);
}

void TeenyPulser::pulse_short_repeat(uint8_t pulseCount) {
  pulse_exec(pulseCount, 1, true);
}

void TeenyPulser::pulse_long(uint8_t pulseCount) {
  pulse_exec(pulseCount, _longPulseWidthMultiplier, false);
}

void TeenyPulser::pulse_long_repeat(uint8_t pulseCount) {
  pulse_exec(pulseCount, _longPulseWidthMultiplier, true);
}

void TeenyPulser::pulse_stop() {
  pulse_exec(0, 0, false);
}

void TeenyPulser::pulse_stop_repeat() {
  pulse_exec(0, 0, true);
}

void TeenyPulser::pulse_exec(uint8_t pulseCount, uint32_t pulseWidthMultiplier, bool repeat) {
  _updateInProgress = true;
  if(pulseCount>0) {
    _pulseState = TeenyPulser::PS_START;
    _pulseCount = pulseCount;
    _pulseWidthMultiplier = max(pulseWidthMultiplier, 1);
    _repeat = repeat;
  } else if(pulseCount==0) {
    _pulseState = TeenyPulser::PS_STOP;
    digitalWrite(_pin, _pulseOffLevel);
  }
  _updateInProgress = false;
}

void TeenyPulser::tick() {
  if(_updateInProgress) return;
  uint32_t _nowMS = millis();
  if(_pulseState==TeenyPulser::PS_STOP) return;
  switch (_pulseState) {
    case TeenyPulser::PS_START:
      _pulseState = TeenyPulser::PS_ON;
      _pulseCountdown = _pulseCount;
      _pulseTimer = _nowMS;
      _repeatTimer = _nowMS;
      digitalWrite(_pin, _pulseOnLevel);
      break;
    case TeenyPulser::PS_ON:
      if((_nowMS-_pulseTimer)>=(_pulseWidth*_pulseWidthMultiplier)) {
        _pulseCountdown--;
        _pulseState = TeenyPulser::PS_OFF;
        //_pulseTimer += _pulseWidth;
        _pulseTimer = _nowMS;
        digitalWrite(_pin, _pulseOffLevel);
      }
      break;
    case TeenyPulser::PS_OFF:
      if((_nowMS-_pulseTimer)>=(_pulseInterval*_pulseWidthMultiplier)) {
        if(_pulseCountdown>0) {
          _pulseState = TeenyPulser::PS_ON;
          _pulseTimer = _nowMS;
          digitalWrite(_pin, _pulseOnLevel);
        } else if(_repeat) {
          _pulseState = TeenyPulser::PS_IDLE;
        } else {
          _pulseState = TeenyPulser::PS_STOP;
        }
      }
      break;
    case TeenyPulser::PS_IDLE:
      if(((_nowMS-_repeatTimer)>=_repeatPeriod)) {
        _pulseState = TeenyPulser::PS_ON;
        _pulseCountdown = _pulseCount;
        _pulseTimer = _nowMS;
        _repeatTimer = _nowMS;
        digitalWrite(_pin, _pulseOnLevel);
        break;
      }
  }
}

