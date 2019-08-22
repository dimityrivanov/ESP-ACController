#include "MitsubishiAC.cpp"
#include "ToshibaAC.cpp"
#include "DaikinAC.cpp"
#ifndef AC_EEPROM
#define AC_EEPROM
#include "EEPROMManager.h"
#endif
#include "AsistantSupport.cpp"
#ifndef AC_UNIT
#define AC_UNIT
#include "ACUnit.cpp"
#endif
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#define AC_ADDRESS 0

ACController* controller;
AssitantSupport sinricSupport;
EEPROMManager eepromManager;
ESP8266WebServer HTTP(1900);
JsonObject mLastRedJsonObject;
StaticJsonDocument<200> doc;

void setup() {
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect("ACControl");
  Serial.println("Connected");
  setupDevice();

  config_rest_server_routing();
  sinricSupport.init(controller);
}

void config_rest_server_routing() {
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {

    HTTP.on("/index.html", HTTP_GET, []() {
      //example device id
      //11111111-1111-1dda-1111-cda1e1ee1e1a
      HTTP.send(200, "application/json", "{\"deviceID\": \"<set_device_id here>\"}");
    });
    HTTP.on("/description.xml", HTTP_GET, []() {
      SSDP.schema(HTTP.client());
    });

    HTTP.on("/init", HTTP_POST, setupDevice);
    HTTP.on("/params", HTTP_POST, paramsDevice);
    HTTP.on("/emit", HTTP_GET, emit);

    HTTP.on("/state", HTTP_GET, []() {
      char response[100];
      ACObject obj = eepromManager.readEEPROMObject(AC_ADDRESS);
      int checksum = obj.temp + obj.mode + obj.fanSpeed + obj.powerState;
      sprintf(response, "{\"temp\": %d,\n    \"fanModeValue\": %d,\n    \"fanSpeedValue\": %d,\n    \"powerStateValue\": %d,\n    \"checksum\": %d}", obj.temp, obj.mode, obj.fanSpeed, obj.powerState, checksum);
      HTTP.send(200, "application/json", response);
    });

    HTTP.begin();

    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(1900);
    SSDP.setName("ACControl");
    SSDP.setSerialNumber("<set serial number here>");
    SSDP.setURL("index.html");
    SSDP.setModelName("ACControl");
    SSDP.setModelNumber("<set model number here>");
    SSDP.setModelURL("http://dimityrivanov.github.io/");
    SSDP.setManufacturer("Dimitar Ivanov");
    SSDP.setManufacturerURL("http://dimityrivanov.github.io/");
    //SSDP.setDeviceType("upnp:rootdevice");
    SSDP.setDeviceType("urn:schemas-upnp-org:device:ACControl:1");
    SSDP.begin();
    Serial.println("ssdp start");
  } else {
    Serial.println("ERROR");
    while (1) {
      delay(100);
    }
  }
}

void request_response(bool positive) {
  if (positive) {
    HTTP.send(200, "application/json", "{\"success\" : true}");
  } else {
    HTTP.send(400, "application/json", "{\"success\" : false}");
  }
}

void parseRequestBody() {

  //request_authorization();

  doc.clear();
  String post_body = HTTP.arg("plain");

  DeserializationError error = deserializeJson(doc, post_body);

  if (error) {
    request_response(false);
  }

  mLastRedJsonObject = doc.as<JsonObject>();
}

boolean checkForKey(const char* key) {
  if (!mLastRedJsonObject.containsKey(key)) {
    Serial.println("wrong json format for request!!!");
    request_response(false);
    return false;
  }
  return true;
}

void setupDevice() {
  ACObject obj = eepromManager.readEEPROMObject(AC_ADDRESS);
  int ac_device_id = -1;
  Serial.println(obj.ac_device);

  if (obj.ac_device != -1) {
    ac_device_id = obj.ac_device;
  } else {
    parseRequestBody();

    if (!checkForKey("ac_device")) {
      Serial.println("ac_device key does not exist!");
      return;
    }

    ac_device_id = mLastRedJsonObject["ac_device"];
    Serial.print("Received data for AC: ");
    Serial.println(ac_device_id);

    ACObject customVar;
    customVar.ac_device = ac_device_id;
    customVar.temp = 0;
    customVar.mode = 0;
    customVar.fanSpeed = 0;
    customVar.powerState = 0;
    Serial.print("Saving data: ");
    Serial.println(customVar.ac_device);
    eepromManager.writeEEPROMObject(AC_ADDRESS, customVar);

    request_response(true);
  }

  ACUnit ac_unit = static_cast<ACUnit>(ac_device_id);

  controller = make_ac(ac_unit);
  controller->initialize();
  controller->toogle(true);
}

void emit() {
  controller->emit();
  request_response(true);
}

void paramsDevice() {
  parseRequestBody();

  if (!checkForKey("temp")) {
    Serial.println("temp key does not exist!");
    return;
  }

  const int temp = mLastRedJsonObject["temp"];
  controller->setTemp(temp);
  const int fanMode = mLastRedJsonObject["mode"];
  ACMode acFanMode = static_cast<ACMode>(fanMode);
  controller->setMode(acFanMode);
  const int fanSpeed = mLastRedJsonObject["fan"];
  controller->setFanSpeed(fanSpeed);
  const int powerState = mLastRedJsonObject["power"];
  bool isOn = (powerState > 0 ? true : false);
  controller->toogle(isOn);
  
  eepromManager.updateAcState(temp, fanMode, fanSpeed, powerState);

  request_response(true);
}

void loop() {
  HTTP.handleClient();
  sinricSupport.loopSocket();
  delay(1);
}

void convertIntToACMode(int modeAsNumber) {
  ACMode mode = static_cast<ACMode>(modeAsNumber);
  controller->setMode(mode);
}

ACController *make_ac(ACUnit type) {
  switch (type) {
    case AC_MITSUBISHI:
      return new MitsubishiAC;

    case AC_TOSHIBA:
      return new ToshibaAC;

    case AC_DAIKIN:
      return new DaikinAC;

    default:
      return new MitsubishiAC;
  }
};
