#include "Arduino.h"
#include "EEPROMManager.h"
#include <EEPROM.h>
#define AC_ADDRESS 0

int EEPROMManager::findEmptyAddress() {
  for (int i = 0; i < EEPROM_SIZE; i += sizeof(ACObject)) {
    int status = getEE(i, EMPRY_ADDRESS);
    if (status == EMPRY_ADDRESS) {
      return i;
    }
  }

  return NULL_ADDRESS;
}

void EEPROMManager::updateAcState(int temp, int fanMode, int fanSpeed, int powerState) {
  ACObject obj = readEEPROMObject(AC_ADDRESS);
  obj.temp = temp;
  obj.mode = fanMode;
  obj.fanSpeed = fanSpeed;
  obj.powerState = powerState;
  writeEEPROMObject(AC_ADDRESS, obj);
}

int EEPROMManager::getEE(int addr, int def) {
  int val = def;
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(addr, val);
  EEPROM.commit();
  EEPROM.end();
  if (val == 255) return def;
  return val;
}

ACObject EEPROMManager::readEEPROMObject(int addr) {
  ACObject customVar; //Variable to store custom object read from EEPROM.
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(addr, customVar);
  EEPROM.commit();
  EEPROM.end();

  return customVar;
}

void EEPROMManager::clearEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 255);
  }

  EEPROM.end();
}

void EEPROMManager::clearEEPROMAddr(int addr) {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(addr, EMPTY_EEPROM_CELL);
  EEPROM.commit();
  EEPROM.end();
}

void EEPROMManager::writeEEPROMObject(int addr, ACObject customVar) {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(addr, customVar);
  EEPROM.commit();
  EEPROM.end();
}


//void test() {
//  eepromManager.clearEEPROM();
//
//  PasswordObject customVar = {
//    "1234567891234567891",
//    "1234567891234567891"
//  };
//
//  eepromManager.writeEEPROMObject(0, customVar);
//
//  for (int i = 0; i < EEPROM_SIZE; i += sizeof(PasswordObject)) {
//    if (DEBUG) {
//      Serial.print(i);
//      Serial.print(" : ");
//    }
//    int value = eepromManager.getEE(i, -1);
//    //Serial.println(value);
//    if (value != -1) {
//      PasswordObject obj = readEEPROMObject(i);
//      if (DEBUG) {
//        Serial.println("Read custom object from EEPROM: ");
//        Serial.println(obj.name);
//        Serial.println(obj.password);
//      }
//    }
//  }
//}
