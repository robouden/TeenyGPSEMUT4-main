
/********************************************************************/
void deviceMode_init() {
  char _dispStr[22];
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSEMUL:
      emulatorSerial = &Serial1;
      emulator_setup(*emulatorSerial, EMULATOR_BAUD_RATE);
      break;
  }
}

/********************************************************************/
void deviceMode_end() {
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSEMUL:
      emulator_end();
      break;
  }
  deviceState.DEVICE_MODE = DM_IDLE;
}

