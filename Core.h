#ifndef AMBIENCE_CORE
#define AMBIENCE_CORE

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

// JSON
#include <ArduinoJson.h>


namespace Ambience
{
  // ==================== Globals ==================== //

  // AmbienceNode Software Version
  #define VERSION "1.1"

  // Enables logging over serial port
  #define DEBUG 1

  // Serial BAUD rate
  #define BAUDRATE 115200

  // LED Data pin
  #define DATA_PIN 14

  // Built-in LED pin
  #define BUILTIN_LED 2

  // External LED pin
  #define EXTERN_LED 12 

  // Synchronized LED indicator state
  #define PULL_LED_INDICATOR(STATE) do {digitalWrite(EXTERN_LED, STATE); digitalWrite(BUILTIN_LED, STATE);} while(0)

  // Number of LEDs
  #define NUM_LEDS 10 // Test Strip
  //#define NUM_LEDS 122 // Kitchen Island
  //#define NUM_LEDS 100 // Desk
  //#define NUM_LEDS 100 // TV Stand
  //#define NUM_LEDS 100 // Server Rack
  //#define NUM_LEDS 165 // Outdoor Bar


  // ==================== Hardware ==================== //

  void InitHardware()
  {
    pinMode(EXTERN_LED, OUTPUT);
    pinMode(BUILTIN_LED, OUTPUT);

    // 3 quick LED flashes indicate initial boot
    for (int i = 0; i < 3; i++)
    {
      PULL_LED_INDICATOR(HIGH);
      delay(200);
      PULL_LED_INDICATOR(LOW);
      delay(200);
    }
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

    // Attempt to establish a connection
    int attempts = 0;
    while (WiFi.waitForConnectResult() != WL_CONNECTED) 
    {     
      attempts++;
      LOGW("failed to connect to WiFi with SSID: ");
      LOGW(ssid);
      LOGF(". Attempt %d. Retrying...", attempts);
      delay(500);
      if (attempts > 5) { ESP.restart(); }
    }

    // Success!
    LOGW("Wifi initialized with IP ");
    LOG(WiFi.localIP());
  }
}
#endif
