
/********************************************************************/
// SD Card
/********************************************************************/
bool sdcardEnabled;
#include <SPI.h>
#include <SD.h>
// teensy 4.1 on-board sd card chip select = BUILTIN_SDCARD
const int sdChipSelect = BUILTIN_SDCARD;
File sdFile; // SD file pointer

/********************************************************************/
bool sdcard_setup() {
  if(SD.begin(sdChipSelect)) {
    sdcardEnabled = true;
  } else {
    sdcardEnabled = false;
  }
  return sdcardEnabled;
}

/********************************************************************/
void sdDateTimeCB(uint16_t* date, uint16_t* time) {
  rtc_datetime_t now = getRTCTime(); // get the RTC
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year, now.month, now.day);
  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour, now.minute, now.second);
}

/********************************************************************/
// Config File Save/restore
/********************************************************************/
bool sdcard_deviceStateReset() {
  if(!sdcardEnabled) return false;
  if(SD.exists("TEENYGPS.cfg")) {
    if(!SD.remove("TEENYGPS.cfg")) return false;
  }
  return true;
}
/********************************************************************/
bool sdcard_deviceStateSave() {
  if(!sdcard_deviceStateReset()) return false;
  SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile = SD.open("TEENYGPS.cfg", FILE_WRITE);
  if(!sdFile) return false;
  sdFile.write((uint8_t*)&deviceState, sizeof(deviceState));
  sdFile.close();
  return true;
}
/********************************************************************/
bool sdcard_deviceStateRestore() {
  if((!sdcardEnabled) || (!SD.exists("TEENYGPS.cfg"))) {
    deviceState.DEVICE_MODE = DM_IDLE;
    deviceState.DISPLAYTIMEOUT = 10;
    deviceState.STATUSLED = true;
    return false;
  }
  sdFile = SD.open("TEENYGPS.cfg");
  if(!sdFile) return false;
  if(sdFile.size()!=sizeof(deviceState)) {
    sdcard_deviceStateReset();
    return false;
  }
  if(sdFile.read((uint8_t*)&deviceState, sizeof(deviceState)) < sizeof(deviceState)) {
    return false;
  }
  sdFile.close();
  return true;
}

/********************************************************************/
// UBX Logging File Writer
/********************************************************************/
bool ubxLoggingInProgress = false;
uint8_t ubxLoggingFileNum = 0;
char ubxLoggingFileName[13]={0};
uint16_t ubxLoggingFileWriteCount;
uint16_t ubxLoggingFileWriteFixedCount;
/********************************************************************/
bool sdcard_openLoggingFile() {
  if(!sdcardEnabled) return false;
  sprintf(ubxLoggingFileName, "NAVPVT%02d.log", ubxLoggingFileNum);
  if(SD.exists(ubxLoggingFileName)) {
    if(!SD.remove(ubxLoggingFileName)) return false;
  }
  SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile = SD.open(ubxLoggingFileName, FILE_WRITE);
  if(!sdFile) return false;
  ubxLoggingFileWriteCount = 0;
  ubxLoggingFileWriteFixedCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeLoggingFile(const uint8_t *buf, size_t size) {
  sdFile.write(buf, size);
  //ubxLoggingFileWriteCount++; //done in loop along with fixed count
}
/********************************************************************/
uint16_t sdcard_closeLoggingFile() {
  sdFile.close();
  ubxLoggingFileNum++;
  if(ubxLoggingFileNum > 99) ubxLoggingFileNum = 0;
  return ubxLoggingFileWriteCount;
}

/********************************************************************/
// UBX Log File Reader
/********************************************************************/
char ubxLogFileName[13]={0};
/********************************************************************/
bool sdcard_openLogFile(const uint8_t logFileNum) {
  if(!sdcardEnabled) return false;
  sprintf(ubxLogFileName, "NAVPVT%02d.emu", logFileNum);
  if(!SD.exists(ubxLogFileName)) {
    return false;
  }
  sdFile = SD.open(ubxLogFileName);
  if(!sdFile) return false;
  return true;
}
/********************************************************************/
bool sdcard_readLogFile(uint8_t *buf, size_t size) {
  // make log file a circular buffer 
  if(sdFile.peek() == -1) {
    if(!sdFile.seek(0)) {
      return false;
    }
  }
  if(sdFile.peek() == -1) {
    return false;
  }
  if(sdFile.read(buf, size) < size) {
    return false;
  }
  return true;
}
/********************************************************************/
void sdcard_closeLogFile() {
  sdFile.close();
}

