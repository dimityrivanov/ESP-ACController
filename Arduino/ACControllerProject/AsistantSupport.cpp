#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#ifndef AC_EEPROM
#define AC_EEPROM
#include "EEPROMManager.h"
#endif
#ifndef AC_CONTROLLER
#define AC_CONTROLLER
#include "ACController.cpp"
#endif
#ifndef AC_UNIT
#define AC_UNIT
#include "ACUnit.cpp"
#endif
#define MyApiKey "<replace with sinric API KEY>"
#define THERMOSTATE_DEVICE_ID "<replace with sinric device thermostat id>"
#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 
#define AC_ADDRESS 0

class AssitantSupport {
  private:
    uint64_t heartbeatTimestamp = 0;
    bool isConnected = false;
    WebSocketsClient webSocket;
    ACController* controller;
    EEPROMManager eepromManager;


  public:

    void loopSocket() {
      webSocket.loop();

      if (isConnected) {
        uint64_t now = millis();

        // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
        if ((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");
        }
      }
    };

    void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
      switch (type) {
        case WStype_DISCONNECTED:
          isConnected = false;
          Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
          break;
        case WStype_CONNECTED: {
            isConnected = true;
            Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
            Serial.printf("Waiting for commands from sinric.com ...\n");
          }
          break;
        case WStype_TEXT: {
            Serial.printf("[WSc] get text: %s\n", payload);
            // Example payloads

            // TODO: Thermostat Alexa Support
            // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html
            // {"deviceId": xxxx, "action": "SetTargetTemperature", value: "targetSetpoint": { "value": 20.0, "scale": "CELSIUS"}} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html#settargettemperature
            // {"deviceId": xxxx, "action": "AdjustTargetTemperature", value: "targetSetpointDelta": { "value": 2.0, "scale": "FAHRENHEIT" }} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html#adjusttargettemperature
            // {"deviceId": xxxx, "action": "SetThermostatMode", value: "thermostatMode" : { "value": "COOL" }} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html#setthermostatmode

            StaticJsonDocument<200> doc;
            DeserializationError error = deserializeJson(doc, (char*)payload);

            if (error) {
              return;
            }

            JsonObject json = doc.as<JsonObject>();
            ACObject obj = eepromManager.readEEPROMObject(AC_ADDRESS);

            if (json["deviceId"] == THERMOSTATE_DEVICE_ID) {
              Serial.println(THERMOSTATE_DEVICE_ID);
              if (json["action"] == "action.devices.commands.OnOff") {
                const bool value = (bool)json ["value"]["on"];
                controller->toogle(value);
                eepromManager.updateAcState(obj.temp, obj.mode, obj.fanSpeed, value);
                controller->emit();
                Serial.print("PowerState : ");
                Serial.println(value);
              }

              if (json["action"] == "action.devices.commands.ThermostatTemperatureSetpoint") {
                const int temp = json ["value"]["thermostatTemperatureSetpoint"];
                controller->setTemp(temp);
                eepromManager.updateAcState(temp, obj.mode, obj.fanSpeed, obj.powerState);
                controller->emit();
                Serial.print("New temp : ");
                Serial.println(temp);
              }

              if (json["action"] == "action.devices.commands.ThermostatSetMode") {
                const String mode = json ["value"]["thermostatMode"];
                if (mode == "heat") {
                  ACMode acFanMode = static_cast<ACMode>(4);
                  controller->setMode(acFanMode);
                  eepromManager.updateAcState(obj.temp, 4, obj.fanSpeed, obj.powerState);
                  controller->emit();
                }

                if (mode == "cool") {
                  ACMode acFanMode = static_cast<ACMode>(2);
                  controller->setMode(acFanMode);
                  eepromManager.updateAcState(obj.temp, 2, obj.fanSpeed, obj.powerState);
                  controller->emit();
                }

                Serial.print("New mode : ");
                Serial.println(mode);
              }
            }
          }

          break;
        case WStype_BIN:
          Serial.printf("[WSc] get binary length: %u\n", length);
          break;
      }
    };

    void init(ACController* controller) {
      // server address, port and URL
      webSocket.begin("iot.sinric.com", 80, "/");

      // event handler
      webSocket.onEvent(std::bind(&AssitantSupport::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
      webSocket.setAuthorization("apikey", MyApiKey);

      // try again every 5000ms if connection has failed
      webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
      this->controller = controller;
    };
};
