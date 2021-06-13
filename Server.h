#ifndef AMBIENCE_SERVER
#define AMBIENCE_SERVER

#include "Core.h"
#include "LED.h"

// WebServer Docs: https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/src/WebServer.h
// Response Code Standard: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
// JSON Serialization Docs: https://arduinojson.org/


// WebServer logging
#define LOG_REQUEST(m) LOGF("Ambience Server received a request with URI: %s\n", m)
#define LOG_RESPONSE(c) LOGF("Ambience Server responded with: %d\n", c)
#define LOG_RESPONSE_PAYLOAD(c, p) LOGF("Ambience Server responded with: %d ", c); LOGF("payload: %s\n", p)
#define LOG_MISSING_ARG(m, a) LOGF("Ambience Server missing required arg: %s ", a); LOGF("in URI: %s\n", m)
#define LOG_INVALID_ARG(m, a) LOGF("Ambience Server encountered invalid value for arg: %s ", a); LOGF("in URI: %s\n", m)


namespace Ambience
{

  // ==================== Server ==================== //
  class Server
  {    
    public:
      Server        (LEDStrip* stripToControl);
      ~Server       () {}
      void          Update();

      static void   HandleNotFound();
      static void   HandleRoot();
      static void   HandleSetColor();
      static void   HandleGetColor();
      static void   HandleSetMode();
      static void   HandleGetMode();
      static void   HandleSetBrightness();
      static void   HandleGetBrightness();
      static void   HandleSetActive();
      static void   HandleGetActive();

    private:
      static WebServer* server;
      static LEDStrip* leds;

      static constexpr char* USAGE =
        "Ambience " VERSION "\n"
        "\n\n"
        "WEBSERVER API:\n"
        "  * This is a test";
  };

  WebServer* Server::server = new WebServer(80);
  LEDStrip* Server::leds = nullptr;


  Server::Server(LEDStrip* stripToControl)
  {
    leds = stripToControl;

    // start MDNS and broadcast service
    if (MDNS.begin("esp32." + WiFi.localIP()))
    {
      LOG("mDNS initialized");
      MDNS.addService("ambience", "_http", 80);
    }
    else
    {
      LOG("MDNS failed to start");
    }

    // bind handles
    server->onNotFound(Server::HandleNotFound);
    server->on("/", Server::HandleRoot);
    server->on("/SetColor", Server::HandleSetColor);
    server->on("/GetColor", Server::HandleGetColor);
    server->on("/SetMode", Server::HandleSetMode);
    server->on("/GetMode", Server::HandleGetMode);
    server->on("/SetBrightness", Server::HandleSetBrightness);
    server->on("/GetBrightness", Server::HandleGetBrightness);
    server->on("/SetActive", Server::HandleSetActive);
    server->on("/GetActive", Server::HandleGetActive);

    // start webserver
    server->begin();
    LOG("WebServer initialized");
  }


  void Server::Update()
  {
    server->handleClient();
  }


  // ==================== Server Handles ==================== //

  #define RESPONSE_OK               200     // OK
  #define RESPONSE_BAD_REQUEST      400     // Invalid Arguments / Request Syntax
  #define RESPONSE_PAGE_NOT_FOUND   404     // Invalid Request / Bad URI


  // ==================== Not Found ==================== //

  void Server::HandleNotFound() 
  {
    LOG_REQUEST(server->uri());
    server->send(RESPONSE_PAGE_NOT_FOUND);
    LOG_RESPONSE(RESPONSE_PAGE_NOT_FOUND);
  }


  // ==================== Root ==================== //

  void Server::HandleRoot() 
  {
    LOG_REQUEST(server->uri());
    LOG_RESPONSE(RESPONSE_OK);
    server->send(RESPONSE_OK, "text/plain", USAGE);
  }

  
  // ==================== Colors ==================== //

  void Server::HandleSetColor()
  {
    LOG_REQUEST(server->uri());
    int color = -1, h = -1, s = -1, v = -1;

    // parse args
    for (int i = 0; i < server->args(); i++)
    {
      if (server->argName(i) == "Color")
      {
        int val = server->arg(i).toInt();
        if(val < 1 || val > 3)
        {
          LOG_INVALID_ARG(server->uri(), server->argName(i));
          LOG_RESPONSE(RESPONSE_BAD_REQUEST);
          server->send(RESPONSE_BAD_REQUEST);
          return;
        }
        color = val;
      }

      else if (server->argName(i) == "H" || server->argName(i) == "S" || server->argName(i) == "V")
      {
        int val = server->arg(i).toInt();
        if((val == 0 && server->arg(i) != "0") || val < 0 || val > 255)
        {
          LOG_INVALID_ARG(server->uri(), server->argName(i));
          LOG_RESPONSE(RESPONSE_BAD_REQUEST);
          server->send(RESPONSE_BAD_REQUEST);
          return;
        }
        if (server->argName(i) == "H") { h = val; }
        if (server->argName(i) == "S") { s = val; }
        if (server->argName(i) == "V") { v = val; }
      }
    }

    // check for missing required args
    if (color == -1 || h == -1 || s == -1 || v == 1)
    {
      if (color == -1) { LOG_MISSING_ARG(server->uri(), "Color"); }
      if (h == -1) { LOG_MISSING_ARG(server->uri(), "H"); }
      if (s == -1) { LOG_MISSING_ARG(server->uri(), "S"); }
      if (v == -1) { LOG_MISSING_ARG(server->uri(), "V"); }
      LOG_RESPONSE(RESPONSE_BAD_REQUEST);
      server->send(RESPONSE_BAD_REQUEST);
      return;
    }

    if (color == 1) { leds->Color1 = CHSV(h, s, v); }
    if (color == 2) { leds->Color2 = CHSV(h, s, v); }
    if (color == 3) { leds->Color3 = CHSV(h, s, v); }

    LOG_RESPONSE(RESPONSE_OK);
    server->send(RESPONSE_OK);
  }


  void Server::HandleGetColor()
  {
    LOG_REQUEST(server->uri());

    for (int i = 0; i < server->args(); i++)
    {
      if (server->argName(i) == "Color")
      {
        int colorIndex = server->arg(i).toInt();
        if(colorIndex < 1 || colorIndex > 3)
        {
          LOG_INVALID_ARG(server->uri(), server->argName(i));
          LOG_RESPONSE(RESPONSE_BAD_REQUEST);
          server->send(RESPONSE_BAD_REQUEST);
          return;
        }

        StaticJsonDocument<64> payload;
        JsonArray color = payload.createNestedArray("Color");
        if (colorIndex == 1) { color.add(leds->Color1.h); color.add(leds->Color1.s); color.add(leds->Color1.v); }
        if (colorIndex == 2) { color.add(leds->Color2.h); color.add(leds->Color2.s); color.add(leds->Color2.v); }
        if (colorIndex == 3) { color.add(leds->Color3.h); color.add(leds->Color3.s); color.add(leds->Color3.v); }
        String serializedPayload;
        serializeJson(payload, serializedPayload);
        LOG_RESPONSE_PAYLOAD(RESPONSE_OK, serializedPayload);
        server->send(RESPONSE_OK, "json", serializedPayload);
        return;
      }
    }

    LOG_MISSING_ARG(server->uri(), "Color");
    LOG_RESPONSE(RESPONSE_BAD_REQUEST);
    server->send(RESPONSE_BAD_REQUEST);
  }


  // ==================== Modes ==================== //

  void Server::HandleSetMode()
  {
    LOG_REQUEST(server->uri());

    for (int i = 0; i < server->args(); i++)
    {
      if (server->argName(i) == "Mode")
      {
        if (leds->SetMode(server->arg(i)))
        {
          LOG_RESPONSE(RESPONSE_OK);
          server->send(RESPONSE_OK);
          return;
        }
        else
        {
          // invalid mode arg
          LOG_INVALID_ARG(server->uri(), server->argName(i));
          LOG_RESPONSE(RESPONSE_BAD_REQUEST);
          server->send(RESPONSE_BAD_REQUEST);
          return;
        }
      }
    }

    LOG_MISSING_ARG(server->uri(), "Mode");
    LOG_RESPONSE(RESPONSE_BAD_REQUEST);
    server->send(RESPONSE_BAD_REQUEST);
  }


  void Server::HandleGetMode()
  {
    LOG_REQUEST(server->uri());
    StaticJsonDocument<64> payload;
    payload["Mode"] = leds->GetMode();
    String serializedPayload;
    serializeJson(payload, serializedPayload);
    LOG_RESPONSE_PAYLOAD(RESPONSE_OK, serializedPayload);
    server->send(RESPONSE_OK, "json", serializedPayload);
  }


  // ==================== Brightness ==================== //

  void Server::HandleSetBrightness()
  {
    LOG_REQUEST(server->uri());

    for (int i = 0; i < server->args(); i++)
    {
      if (server->argName(i) == "Brightness")
      {
        int val = server->arg(i).toInt();
        if((val == 0 && server->arg(i) != "0") || val < 0 || val > 100)
        {
          LOG_INVALID_ARG(server->uri(), server->argName(i));
          LOG_RESPONSE(RESPONSE_BAD_REQUEST);
          server->send(RESPONSE_BAD_REQUEST);
          return;
        }
        leds->SetBrightness(val * 0.01);
        LOG_RESPONSE(RESPONSE_OK);
        server->send(RESPONSE_OK);
        return;
      }
    }
  }


  void Server::HandleGetBrightness()
  {
    LOG_REQUEST(server->uri());
    StaticJsonDocument<16> payload;
    payload["Brightness"] = (int)(leds->GetBrightness() * 100);
    String serializedPayload;
    serializeJson(payload, serializedPayload);
    LOG_RESPONSE_PAYLOAD(RESPONSE_OK, serializedPayload);
    server->send(RESPONSE_OK, "json", serializedPayload);
  }


  // ==================== Active ==================== //

  void Server::HandleSetActive()
  {
    LOG_REQUEST(server->uri());
    
    for (int i = 0; i < server->args(); i++)
    {
      if (server->argName(i) == "Active")
      {
        if (server->arg(i) == "True") 
        { 
          leds->SetActive(true);
          LOG_RESPONSE(RESPONSE_OK);
          server->send(RESPONSE_OK);
          return;
        }
        if (server->arg(i) == "False") 
        { 
          leds->SetActive(false);
          LOG_RESPONSE(RESPONSE_OK);
          server->send(RESPONSE_OK);
          return;
        }
        LOG_INVALID_ARG(server->uri(), server->argName(i));
        LOG_RESPONSE(RESPONSE_BAD_REQUEST);
        server->send(RESPONSE_BAD_REQUEST);
        return;
      }
    }
    LOG_MISSING_ARG(server->uri(), "Active");
    LOG_RESPONSE(RESPONSE_BAD_REQUEST);
    server->send(RESPONSE_BAD_REQUEST);
  }


  void Server::HandleGetActive()
  {
    LOG_REQUEST(server->uri());
    StaticJsonDocument<16> payload;
    payload["Active"] = leds->GetActive();
    String serializedPayload;
    serializeJson(payload, serializedPayload);
    LOG_RESPONSE_PAYLOAD(RESPONSE_OK, serializedPayload);
    server->send(RESPONSE_OK, "json", serializedPayload);
  }
}
#endif
