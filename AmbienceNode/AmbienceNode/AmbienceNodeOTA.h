#ifndef AMBIENCE_NODE_OTA
#define AMBIENCE_NODE_OTA

#include "AmbienceNodeCore.h"

void InitOTA() {
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      }
      else { // U_SPIFFS
        type = "filesystem";
      }
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      LOG("Start updating " + type);
    })
    .onEnd([]() {
      LOG("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      LOGF("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      LOGF("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) LOG("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) LOG("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) LOG("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) LOG("Receive Failed");
      else if (error == OTA_END_ERROR) LOG("End Failed");
    });

  ArduinoOTA.begin();

  LOG("OTA Initialized");
}

void UpdateOTA() {
  ArduinoOTA.handle();
}

#endif
