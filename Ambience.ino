#include "Core.h"
#include "OTA.h"
#include "LED.h"
#include "Server.h"

Ambience::LEDStrip* Leds;
Ambience::OTA* Ota;
Ambience::Server* Server;

void setup() 
{ 
  Ambience::InitHardware();
  Ambience::InitLogging();
  Ambience::InitWiFi();

  Leds = new Ambience::LEDStrip();
  Ota = new Ambience::OTA();
  Server = new Ambience::Server(Leds);
}

void loop() 
{
  Ota->Update();
  Server->Update();
  Leds->Update();
}
