#ifndef AMBIENCE_NODE_WEBSERVER
#define AMBIENCE_NODE_WEBSERVER

#include "AmbienceNodeCore.h"

// WebServer logging
#define LOG_REQUEST(m) LOGF("AmbienceNodeWebServer received a request with URI: %s\n", m)
#define LOG_RESPONSE(c) LOGF("AmbienceNodeWebServer responded with: %d\n", c)
#define LOG_UNEXPECTED_ARG(m, a) LOGF("AmbienceNodeWebServer encountered an unexpected arg: %s ", a); LOGF("in URI: %s\n", m)
#define LOG_MISSING_ARG(m, a) LOGF("AmbienceNodeWebServer missing required arg: %s ", a); LOGF("in URI: %s\n", m)


namespace AmbienceNodeWebServer
{
  WebServer server(80);


  // ==================== WebServer Handles ==================== //

  // WebServer Docs: https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/src/WebServer.h
  // Response Code Standard: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
  /*
      200 = OK
      404 = Page Not Found (Invalid Request / Bad URI)
      400 = Bad Request (Invalid Arguments / Request Syntax)
  */

  void HandleRoot() 
  {
    LOG_REQUEST(server.uri());

    const char* usage = 
      "AmbienceNode " VERSION "\n"
      "\n\n"
      "WEBSERVER API:\n"
      "  * This is a test";
    
    LOG_RESPONSE(200);
    server.send(200, "text/plain", usage);
  }

  void HandleSetColor()
  {
    LOG_REQUEST(server.uri());

    int h = -1, s = -1, v = -1;
    for (int i = 0; i < server.args(); i++)
    {
      // Validate arg
      int val;

      try
      {
        val = std::atoi(server.arg(i).c_str());
        if (val < 0 || val > 255)
        {
          throw 1;
        }
      }

      catch (...)
      {
        LOG_RESPONSE(400);
        server.send(400);
        return;
      }

      // Parse and assign arg
      if (server.argName(i) == "h")
      {
        h = val;
      }
      else if (server.argName(i) == "s")
      {
        s = val;
      }
      else if (server.argName(i) == "v")
      {
        v = val;
      }
      else
      {
        LOG_UNEXPECTED_ARG(server.uri(), server.arg(i).c_str());
      }
    }

    // Check for missing required args
    if (h == -1 || s == -1 || v == 1)
    {
      if (h == -1)
      {
        LOG_MISSING_ARG(server.uri(), "h");
      }
      if (s == -1)
      {
        LOG_MISSING_ARG(server.uri(), "s");
      }
      if (v == -1)
      {
        LOG_MISSING_ARG(server.uri(), "v");
      }

      LOG_RESPONSE(400);
      server.send(400);
      return;
    }

    /* Call LED functions here... */
    LOG_RESPONSE(200);
    server.send(200);
  }

  void HandleNotFound() 
  {
    LOG_REQUEST(server.uri());
    server.send(404);
    LOG_RESPONSE(404);
  }


  // ==================== WebServer ==================== //

  void Init() 
  {
    if (!MDNS.begin("esp32")) 
    {
      LOG("MDNS failed to start");
    }

    // bind handles
    server.on("/", HandleRoot);
    server.on("/SetColor", HandleSetColor);
    server.onNotFound(HandleNotFound);

    // start webserver
    server.begin();
    LOG("WebServer initialized");
  }

  void Update() 
  {
    server.handleClient();
  }
}
#endif
