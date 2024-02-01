// TeenyGPSEmulator Arduino Sketch
// 2023-12-08 by Michael Beakes <beakes@bezwax.com>
//
// Changelog:
// 2023-03-17 - Initial release
//
// To-Do:
//

#include <Arduino.h>

/********************************************************************/
// Reset
void device_reset() {
  // send reboot command -----
  SCB_AIRCR = 0x05FA0004; // teensy4.1
}

/********************************************************************/
// Device Modes
enum device_mode_t : int {
  DM_IDLE = 0,
  DM_GPSRCVR,
  DM_GPSLOGR,
  DM_GPSCAPT,
  DM_GPSSSTP,
  DM_GPSEMUL
};

/********************************************************************/
// Device State
typedef struct {
  int      DEVICE_MODE = DM_IDLE;
  int      EMUL_NUMCOLDSTARTPVTPACKETS = 10;
  int      DISPLAYTIMEOUT = 10;
  bool     STATUSLED = true;
  uint8_t  pad0;
  uint8_t  pad1;
  uint8_t  pad2;
} device_state_t;
device_state_t deviceState;

/********************************************************************/
// Prototypes
#include "prototypes.h"

/********************************************************************/
// RTC
#include "rtc.h"

/********************************************************************/
// SD Card
#include "sdcard.h"

/********************************************************************/
// Emulator
HardwareSerial *emulatorSerial;
#include "emulator.h"

/********************************************************************/
// Device Mode
#include "mode.h"

/********************************************************************/
// Pulsed Outputs
#include <TeenyPulser.h>
// Status LED Defines
TeenyPulser statusLED(13, true, 20, 50, 4000);

/********************************************************************/
// Periodic Timer
#include "timer.h"


/********************************************************************/
void setup() {

  // setup statusLED
  statusLED.init();

  //Setup rtc
  rtc_setup();

  //Setup sdcard
  sdcard_setup();

  //Select device mode
  deviceState.DEVICE_MODE = DM_GPSEMUL;
  deviceMode_init();

  //status
  statusLED.pulse_repeat(1);

  // setup timer
  itimer0_setup();

}


/********************************************************************/
void loop() {

  // Update clock
  rtc_datetime_t _rtcTime = getRTCTime(); // get the RTC
  uint32_t _clockTime = (uint32_t)(_rtcTime.hour*3600) + (uint32_t)(_rtcTime.minute*60) + _rtcTime.second;
  static uint32_t _prevClockTime = 86400; // This is 24hr rollover seconds so it will never match _clockTime
  bool _clockTick_1sec = false;
  if((_prevClockTime) != (_clockTime)) {
    _prevClockTime = _clockTime;
    _clockTick_1sec = true;
  }

  // Update device based on mode
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSEMUL:
      // Process host commands
      emulator.processIncomingPacket();
      // Transmit autoPVT packets if enabled
//*** NEED TO INCORPORATE TRANSMISSION RATE INTO sendPVTPacket()
//*** ALSO NEED TO FACTOR IN LOG RATE VS TRANSMISSION RATE
//uint32_t getPVTTransmissionRate();
      uint8_t _ubxPVTBuf[100];
      ubxPVTInfo_t _ubxPVTInfo;
      static uint8_t _coldPVTPacketCount = 0;
      if(_clockTick_1sec &&
         (emulator.isAutoPVTEnabled() || emulator.isPVTPacketRequested())) {
        if(!clockTime_valid) {
          if(_coldPVTPacketCount < deviceState.EMUL_NUMCOLDSTARTPVTPACKETS) {
            _coldPVTPacketCount++;
            emulator.setPVTColdPacket();
            emulator.sendPVTPacket();
            statusLED.pulse(1);
          } else {
            if(emulatorSDCardEnabled &&
               sdcard_readLogFile(_ubxPVTBuf, sizeof(_ubxPVTBuf))) {
              emulator.setPVTPacket(_ubxPVTBuf, sizeof(_ubxPVTBuf));
            } else {
              emulator.setPVTLoopPacket();
            }
            _ubxPVTInfo = emulator.getPVTPacketInfo();
            if(_ubxPVTInfo.dateValid && _ubxPVTInfo.timeValid) {
              setRTCTime(_ubxPVTInfo.hour, _ubxPVTInfo.minute, _ubxPVTInfo.second,
                         _ubxPVTInfo.day, _ubxPVTInfo.month, _ubxPVTInfo.year);
              clockTime_valid = true;
              _prevClockTime = (uint32_t)(_ubxPVTInfo.hour*3600) +
                               (uint32_t)(_ubxPVTInfo.minute*60) +
                               _ubxPVTInfo.second;
            }
            emulator.sendPVTPacket();
            statusLED.pulse(_ubxPVTInfo.locationValid ? 2 : 1);
          }
        } else {
          if(emulatorSDCardEnabled &&
             sdcard_readLogFile(_ubxPVTBuf, sizeof(_ubxPVTBuf))) {
            emulator.setPVTPacket(_ubxPVTBuf, sizeof(_ubxPVTBuf));
          } else {
            emulator.setPVTLoopPacket();
          }
          _ubxPVTInfo = emulator.getPVTPacketInfo();
          emulator.setPVTPacketDateTime(_rtcTime.year, _rtcTime.month, _rtcTime.day,
                                        _rtcTime.hour, _rtcTime.minute, _rtcTime.second);
          emulator.sendPVTPacket();
          statusLED.pulse(_ubxPVTInfo.locationValid ? 2 : 1);
        }
      }
      break;
  }


}

