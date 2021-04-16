#ifndef AMBIENCE_NODE_WEBSERVER
#define AMBIENCE_NODE_WEBSERVER

#include "AmbienceNodeCore.h"
WebServer server(80);


// ==================== Webserver Handles ==================== //

void HandleWebserverRoot() 
{
  const char* usage = 
    "AmbienceNode " VERSION "\n"
    "\n\n"
    "WEBSERVER API:\n"
    "  * This is a test";
  
  server.send(200, "text/plain", usage);
}

void HandleWebserverNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


// ==================== Webserver ==================== //

void InitWebserver() 
{
  if (!MDNS.begin("esp32")) {
    LOG("MDNS failed to start");
  }

  // bind handles
  server.on("/", HandleWebserverRoot);
  server.on("/red", HandleWebserverRoot);
  server.onNotFound(HandleWebserverNotFound);

  // start webserver
  server.begin();
  LOG("Webserver initialized");
}

void UpdateWebserver() 
{
  server.handleClient();
}


#endif
