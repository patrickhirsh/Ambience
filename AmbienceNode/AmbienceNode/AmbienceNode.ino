#include "AmbienceNodeCore.h"
#include "AmbienceNodeOTA.h"
#include "AmbienceNodeLED.h"
#include "AmbienceNodeWebServer.h"


void setup() 
{ 
  AmbienceNodeCore::InitHardware();
  AmbienceNodeCore::InitLogging();
  AmbienceNodeCore::InitWiFi();
  AmbienceNodeLED::Init();
  AmbienceNodeOTA::Init();
  AmbienceNodeWebServer::Init();
}

void loop() 
{
  AmbienceNodeOTA::Update();
  AmbienceNodeWebServer::Update();
  AmbienceNodeLED::Update();
}
