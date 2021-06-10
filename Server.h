#ifndef AMBIENCE_SERVER
#define AMBIENCE_SERVER

#include "Core.h"
#include "LED.h"

// WebServer logging
#define LOG_REQUEST(m) LOGF("Ambience WebServer received a request with URI: %s\n", m)
#define LOG_RESPONSE(c) LOGF("Ambience WebServer responded with: %d\n", c)
#define LOG_MISSING_ARG(m, a) LOGF("Ambience WebServer missing required arg: %s ", a); LOGF("in URI: %s\n", m)
#define LOG_INVALID_ARG(m, a) LOGF("Ambience WebServer encountered invalid value for arg: %s ", a); LOGF("in URI: %s\n", m)


namespace Ambience
{
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

  WebServer* Server::server = nullptr;
  LEDStrip* Server::leds = nullptr;


  Server::Server(LEDStrip* stripToControl)
  {
    if (server == nullptr)
    {
      server = new WebServer(80);
    }
    leds = stripToControl;

    if (!MDNS.begin("esp32")) 
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
    server->on("/GetActive", Server::HandleSetActive);

    // start webserver
    server->begin();
    LOG("WebServer initialized");
  }

  void Server::Update()
  {
    server->handleClient();
  }

  // ==================== Server Handles ==================== //

  // WebServer Docs: https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/src/WebServer.h
  // Response Code Standard: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
  #define RESPONSE_OK               200
  #define RESPONSE_BAD_REQUEST      400     // Invalid Arguments / Request Syntax
  #define RESPONSE_PAGE_NOT_FOUND   404     // BInvalid Request / Bad URI


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
    // TODO: Implement This
  }


  // ==================== Modes ==================== //

  void Server::HandleSetMode()
  {
    LOG_REQUEST(server->uri());

    for (int i = 0; i < server->args(); i++)
    {
      if (server->argName(i) == "Mode")
      {
        
        // *** Mode Mapping *** //

        if (server->arg(i) == "Color") 
        {  
          leds->SetMode(LEDStrip::Mode::Color);
          LOG_RESPONSE(RESPONSE_OK);
          server->send(RESPONSE_OK);
          return;
        }

        // invalid mode
        LOG_INVALID_ARG(server->uri(), server->argName(i));
        LOG_RESPONSE(RESPONSE_BAD_REQUEST);
        server->send(RESPONSE_BAD_REQUEST);
        return;
      }
    }

    LOG_MISSING_ARG(server->uri(), "Mode");
    LOG_RESPONSE(RESPONSE_BAD_REQUEST);
    server->send(RESPONSE_BAD_REQUEST);
  }

  void Server::HandleGetMode()
  {
    // TODO: Implement This
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
    // TODO: Implement This
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
    // TODO: Implement This
  }

}
#endif
