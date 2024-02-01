
/********************************************************************/
// Timer
// Teensy 4.1 allows up to four timers
IntervalTimer ITimer0;
volatile uint8_t newClockSubTickCount = 0;
volatile bool newClockTick = false;

/********************************************************************/
// Timer Handler
void itimer0_handler() {
  newClockSubTickCount++;
  if(newClockSubTickCount>=100) {
    newClockSubTickCount = 0;
    newClockTick = true;
  }
  if(deviceState.STATUSLED) statusLED.tick(); //update every 10ms
  switch(newClockSubTickCount%4) {
    case 0:
      if(deviceState.DEVICE_MODE==DM_GPSEMUL) emulator.tick(); //update every 40ms
      break;
  }
}

/********************************************************************/
void itimer0_setup() {
  ITimer0.begin(itimer0_handler, 10000); // 10ms interval
  //ITimer0.priority(0); // need to set if there are two or more timers
}

