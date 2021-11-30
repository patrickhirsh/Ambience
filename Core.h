#ifndef AMBIENCE_CORE
#define AMBIENCE_CORE


// ==================== Globals ==================== //

// AmbienceNode Software Version
#define VERSION "1.2"

// Enables logging over serial port
#define DEBUG 1

/* Enables OTA update functionality. If disabled, updates to system software
must be performed over the serial port of the ESP. Requires NETWORK to be
enabled. */
#define USE_OTA 1

/* Enables network/webserver functionality. If this is disabled, Ambience 
Node will operate in offline mode and not attempt to connect to wifi or
initialize the webserver. */
#define USE_NETWORK 1

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

// Hardware flags to pass to the NEOPIXEL library (See Adafruit_NeoPixel.h)
#define NEOPIXEL_FLAGS (NEO_GRBW + NEO_KHZ800)


// ==================== LED Count ==================== //

/* Number of LEDs installed in a variety of locations in my apartment.
NUM_LEDS should be definied as the corresponding definition befow when
flashing firmware to that node. */
#define LED_COUNT_TEST_STRIP 10
#define LED_COUNT_KITCHEN_ISLAND 122
#define LED_COUNT_DESK 100
#define LED_COUNT_TV_STAND 100
#define LED_COUNT_SERVER_RACK 100

// Number of LEDs
#define NUM_LEDS LED_COUNT_TEST_STRIP

// ESP libraries
#if USE_OTA
#include <ArduinoOTA.h>     // Requires ArduinoOTA by Juraj Andrassy
#endif // USE_OTA
#if USE_NETWORK
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#endif // USE_NETWORK

// NeoPixel
#include <Adafruit_NeoPixel.h>

// JSON
#include <ArduinoJson.h>


namespace Ambience
{

  // ==================== Hardware ==================== //

  void InitHardware()
  {
    pinMode(EXTERN_LED, OUTPUT);
    pinMode(BUILTIN_LED, OUTPUT);

    // 3 quick LED flashes indicate boot sequence start
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
#endif // AMBIENCE_CORE
