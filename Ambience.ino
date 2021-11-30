#include "Core.h"
#include "OTA.h"
#include "LED.h"
#include "Server.h"


Ambience::LEDStrip* Leds;

#if USE_NETWORK && USE_OTA
Ambience::OTA* Ota;
#endif // USE_NETWORK && USE_OTA

#if USE_NETWORK
Ambience::Server* Server;
#endif // USE_NETWORK


void setup() 
{
  // Init local hardware and led systems
  Ambience::InitHardware();
  Ambience::InitLogging();
  Leds = new Ambience::LEDStrip();

  // Init network systems
  #if NETWORK
  Ambience::InitWiFi();
  Server = new Ambience::Server(Leds);
  #endif // NETWORK

  // Init OTA systems
  #if NETWORK && OTA
  Ota = new Ambience::OTA();
  #endif // NETWORK && OTA
}

void loop() 
{
#if USE_NETWORK && USE_OTA
  Ota->Update();
#endif // USE_NETWORK && USE_OTA

#if NETWORK
  Server->Update();
#endif // USE_NETWORK

  Leds->Update();
}
