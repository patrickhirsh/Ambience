#if USE_NETWORK
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


  // ==================== Usage ==================== //

      static constexpr char* USAGE =
        "\n"
        "Ambience Device Version " VERSION "\n"
        "\n"
        "\n"
        "\n"
        "API Reference:\n"
        "\n"
        "\n"
        "  /SetColor - sets the color (HSVW or HSV value) of the color indexed at 'Color'\n"
        "    - Color=<1..3>\n"
        "    - H=<0..65535>\n"
        "    - S=<0..255>\n"
        "    - V=<0..255>\n"
        "    - W=<0..255> (Optional)\n"
        "\n"
        "      RETURNS: none\n"
        "\n"
        "\n"
        "  /GetColor - gets the color (HSVW value) of the color indexed at 'Color'\n"
        "    - Color=<1..3>\n"
        "\n"
        "      RETURNS: {'Color': [<0..65535>,<0..255>,<0..255>,<0.255>]}\n"
        "\n"
        "\n"
        "  /SetMode - sets the mode. Expects a valid string 'Mode' name\n"
        "    - Mode=<Mode>\n"
        "\n"
        "      RETURNS: none\n"
        "\n"
        "\n"
        "  /GetMode - gets the current mode\n"
        "\n"
        "      RETURNS: {'Mode':<Mode>}\n"
        "\n"
        "\n"
        "  /SetBrightness - sets the brightness\n"
        "    - Brightness=<0..255>\n"
        "\n"
        "      RETURNS: none\n"
        "\n"
        "\n"
        "  /GetBrightness - gets the current brightness\n"
        "\n"
        "      RETURNS: {'Brightness':<0..255>}\n"
        "\n"
        "\n"
        "  /SetActive - sets the active (power) state of the leds\n"
        "    - Active=<bool>\n"
        "\n"
        "      RETURNS: none\n"
        "\n"
        "\n"
        "  /GetActive - gets the active (power) state of the leds\n"
        "\n"
        "      RETURNS: {'Active':<bool>}\n";
  };

  WebServer* Server::server = new WebServer(80);
  LEDStrip* Server::leds = nullptr;


  Server::Server(LEDStrip* stripToControl)
  {
    leds = stripToControl;

    // TODO: Possibly support mDNS device discovery?
    /*
    // start MDNS and broadcast service
    if (MDNS.begin("Ambience." + WiFi.localIP()))
    {
      LOG("mDNS initialized");
      MDNS.addService("_ambience", "_tcp", 80);
    }
    else
    {
      LOG("MDNS failed to start");
    }
    */

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
    int color = -1, h = -1, s = -1, v = -1, w = -1;

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

      // Hue should fit in a uint16_t
      else if (server->argName(i) == "H")
      {
        int val = server->arg(i).toInt();
        if((val == 0 && server->arg(i) != "0") || val < 0 || val > 65535)
        {
          LOG_INVALID_ARG(server->uri(), server->argName(i));
          LOG_RESPONSE(RESPONSE_BAD_REQUEST);
          server->send(RESPONSE_BAD_REQUEST);
          return;
        }
        h = val;
      }

      // Saturation, Value, and White should fit in a uint8_t
      else if (server->argName(i) == "S" || server->argName(i) == "V" || server->argName(i) == "W")
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
        if (server->argName(i) == "W") { w = val; }
      }
    }

    // check for missing required args
    if (color == -1 || h == -1 || s == -1 || v == 1)
    {
      if (color == -1) { LOG_MISSING_ARG(server->uri(), "Color"); }
      if (h == -1) { LOG_MISSING_ARG(server->uri(), "H"); }
      if (s == -1) { LOG_MISSING_ARG(server->uri(), "S"); }
      if (v == -1) { LOG_MISSING_ARG(server->uri(), "V"); }
      // Note, "W" is optional
      LOG_RESPONSE(RESPONSE_BAD_REQUEST);
      server->send(RESPONSE_BAD_REQUEST);
      return;
    }

    if (w == -1)
    {
      // white channel not specified
      if (color == 1) { leds->Color1 = LEDStrip::Color(h, s, v); }
      if (color == 2) { leds->Color2 = LEDStrip::Color(h, s, v); }
      if (color == 3) { leds->Color3 = LEDStrip::Color(h, s, v); }
    }
    else
    {
      if (color == 1) { leds->Color1 = LEDStrip::Color(h, s, v, w); }
      if (color == 2) { leds->Color2 = LEDStrip::Color(h, s, v, w); }
      if (color == 3) { leds->Color3 = LEDStrip::Color(h, s, v, w); }
    }

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
        if (colorIndex == 1) { color.add(leds->Color1.H); color.add(leds->Color1.S); color.add(leds->Color1.V); color.add(leds->Color1.W); }
        if (colorIndex == 2) { color.add(leds->Color2.H); color.add(leds->Color2.S); color.add(leds->Color2.V); color.add(leds->Color2.W); }
        if (colorIndex == 3) { color.add(leds->Color3.H); color.add(leds->Color3.S); color.add(leds->Color3.V); color.add(leds->Color3.W); }
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
        if((val == 0 && server->arg(i) != "0") || val < 0 || val > 255)
        {
          LOG_INVALID_ARG(server->uri(), server->argName(i));
          LOG_RESPONSE(RESPONSE_BAD_REQUEST);
          server->send(RESPONSE_BAD_REQUEST);
          return;
        }
        leds->SetBrightness(val);
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
    payload["Brightness"] = (int)(leds->GetBrightness());
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
#endif // AMBIENCE_SERVER
#endif // USE_NETWORK
