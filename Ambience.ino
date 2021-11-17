#include "Core.h"
#include "OTA.h"
#include "LED.h"
#include "Server.h"

Ambience::LEDStrip* Leds;
Ambience::OTA* Ota;
Ambience::Server* Server;

void setup() 
{ 
  // Initialize local hardware and LEDs first
  Ambience::InitHardware();
  Ambience::InitLogging();
  Leds = new Ambience::LEDStrip();

  // Initializes network and server communications
  Ambience::InitWiFi();
  Ota = new Ambience::OTA();
  Server = new Ambience::Server(Leds);

}

void loop() 
{
  Ota->Update();
  Server->Update();
  Leds->Update();
}
