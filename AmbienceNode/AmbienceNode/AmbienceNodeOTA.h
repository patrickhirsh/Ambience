#ifndef AMBIENCE_NODE_OTA
#define AMBIENCE_NODE_OTA

#include "AmbienceNodeCore.h"


namespace AmbienceNodeOTA
{
  void Init() 
  {
    ArduinoOTA

      .onStart([]() 
      {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
          type = "sketch";
        }
        else { // U_SPIFFS
          type = "filesystem";
        }
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        LOG("Start updating " + type);

        // 2 long LED flashes indicate begin OTA
        delay(2000);
        for (int i = 0; i < 2; i++)
        {
          digitalWrite(BUILTIN_LED, HIGH);
          delay(500);
          digitalWrite(BUILTIN_LED, LOW);
          delay(500);
        }

        // Leave LED on during OTA update
        digitalWrite(BUILTIN_LED, HIGH);
      })

      .onEnd([]() 
      {
        LOG("\nEnd");
        digitalWrite(BUILTIN_LED, LOW);
      })

      .onProgress([](unsigned int progress, unsigned int total) 
      {
        LOGF("Progress: %u%%\r", (progress / (total / 100)));
      })

      .onError([](ota_error_t error) 
      {
        LOGF("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) LOG("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) LOG("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) LOG("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) LOG("Receive Failed");
        else if (error == OTA_END_ERROR) LOG("End Failed");

        // two quick flashes indicate failure
        digitalWrite(BUILTIN_LED, LOW);
        delay(1000);
        for (int i = 0; i < 2; i++)
        {
          digitalWrite(BUILTIN_LED, HIGH);
          delay(250);
          digitalWrite(BUILTIN_LED, LOW);
          delay(250);
        }
      });

    ArduinoOTA.begin();
    LOG("OTA Initialized");
  }

  void Update() 
  {
    ArduinoOTA.handle();
  }
}
#endif
