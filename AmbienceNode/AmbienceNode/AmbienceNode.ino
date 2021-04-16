#include "AmbienceNodeCore.h"
#include "AmbienceNodeOTA.h"
#include "AmbienceNodeWebserver.h"
#include "AmbienceNodeLED.h"


void setup() 
{
  pinMode(BUILTIN_LED, OUTPUT);
  
  InitLogging();
  InitWiFi();
  InitOTA();
  InitWebserver();
  InitLEDs();
}

void loop() 
{
  UpdateOTA();
  UpdateWebserver();
  UpdateLEDs();
}
