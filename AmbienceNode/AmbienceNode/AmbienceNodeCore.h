#ifndef AMBIENCE_NODE_CORE
#define AMBIENCE_NODE_CORE

// Standard Library
#include <math.h>

// ESP Library
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

// FastLED
#include <FastLED.h>
#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <platforms.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <power_mgt.h>

namespace AmbienceNodeCore
{
  // ==================== Globals ==================== //

  // AmbienceNode Software Version
  #define VERSION "0.01"

  // Enables logging over serial port
  #define DEBUG 1

  // Serial BAUD rate
  #define BAUDRATE 115200

  // LED Data pin
  #define DATA_PIN 14

  // Built-in LED pin
  #define BUILTIN_LED 2

  // Number of LEDs
  #define NUM_LEDS 10


  // ==================== Hardware ==================== //

  void InitHardware()
  {
    pinMode(BUILTIN_LED, OUTPUT);
  }


  // ==================== Logging ==================== //

  void InitLogging() 
  {
    #if DEBUG
    Serial.begin(BAUDRATE);
    delay(100);
    Serial.println("Logging Initialized");
    #endif
  }

  #if DEBUG
  #define LOG(m) ( Serial.println(m) )
  #define LOGW(m) ( Serial.print(m) )
  #define LOGF(m, v) ( Serial.printf(m, v) )
  #else
  #define LOG(m) ({})
  #define LOGW(m) ({})
  #define LOGF(m, v) ({})
  #endif


  // ==================== WiFi ==================== //

  void InitWiFi() 
  {

    // WiFi credentials... TODO: don't store in plain text
    const char* ssid = "HNET2";
    const char* pass = "Flask!Deranged1!Oasis!Jaws";

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    delay(1000);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      LOGW("failed to connect to WiFi with SSID: ");
      LOGW(ssid);
      LOG(". rebooting...");
      delay(5000);
      ESP.restart();
    }
    LOGW("Wifi initialized with IP ");
    LOG(WiFi.localIP());
  }
}
#endif
