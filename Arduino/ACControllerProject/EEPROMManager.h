#ifndef EEPROMManager_h
#define EEPROMManager_h

#include "Arduino.h"
#include "ACObject.cpp"
class EEPROMManager
{
  public:
    int getEE(int addr, int def);
    int findEmptyAddress();
    ACObject readEEPROMObject(int addr);
    void clearEEPROM();
    void clearEEPROMAddr(int addr);
    void updateAcState(int temp, int fanMode, int fanSpeed,int powerState);
    void writeEEPROMObject(int addr, ACObject customVar);
    const int EMPRY_ADDRESS = -1;
    const int NULL_ADDRESS = -2;
  private:
    const int EEPROM_SIZE = 400;
    const int EMPTY_EEPROM_CELL = 255;
};
#endif
