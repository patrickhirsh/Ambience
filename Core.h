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

// Minimum tick time in miliseconds (17ms ~ 60hz, 8ms ~120hz)
#define TICKRATE 8

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
NUM_LEDS should be defined as the corresponding definition befow when
flashing firmware to that node. */
#define NUM_LED_TEST_STRIP_LONG 20
#define NUM_LED_TEST_STRIP 10
#define NUM_LED_KITCHEN_ISLAND 122
#define NUM_LED_DESK 100
#define NUM_LED_TV_STAND 100
#define NUM_LED_SERVER_RACK 100

#include <limits>

// Number of LEDs
#define NUM_LEDS 100

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

  // ==================== Time Management ==================== //

  namespace Timing
  {

  }


  // ==================== Tick Manager ==================== //

  /* A TickManager guarantees a minimum amount of time spent in its critical section between calls to StartTick() and EndTick()
  in order to impose a minimum tick time. This can be used to throttle tick functions. TickManager takes no action if execution 
  in its critical section exceeds the minimum time. */
  class TickManager
  {
    public:
      TickManager           (uint8_t TickTime);   // initializes a TickManager with a minimum tick time (in milliseconds)
      ~TickManager          () {}
      void StartTick        ();                   // Begin TickManager critical section
      void EndTick          ();                   // End TickManager critical section
      uint8_t GetTickTime   ();                   // returns the current ticktime (in milliseconds)
      void SetTickTime      (uint8_t TickTime);   // sets a ticktime (in milliseconds)

    private:
      uint32_t              tickTime;             // minimum time per tick (in microseconds)
      uint64_t              tickStart;            // time at the start of the current tick (in microseconds)
      struct timeval        tv;                   // timeval object

      // profiling
      uint32_t              tickSum;              // sum of tick times used to calculate avg ticktime (in microseconds)
      uint16_t              tickCount;            // number of tick times added to tickSum

  };

  TickManager::TickManager(uint8_t TickTime)
  {
    tickTime = TickTime * 1000;
    tickStart = 0;
    tickSum = 0;
    tickCount = 0;
  }

  void TickManager::StartFrame()
  {
    gettimeofday(&tv, NULL);
    tickStart = (uint64_t)tv.tv_sec * 1000000L + (uint64_t)tv.tv_usec;
  }

  void TickManager::EndFrame()
  {
    gettimeofday(&tv, NULL);
    uint64_t tickEnd = (uint64_t)tv.tv_sec * 1000000L + (uint64_t)tv.tv_usec;
    uint32_t tickDelta = tickEnd - tickStart;
    if (tickDelta < tickTime)
    {
      delay((tickTime - tickDelta) / 1000);
      tickSum += tickTime;
    }
    else
    {
      tickSum += tickDelta;
    }
    tickCount++;

    if (tickCount >= (1000 / (tickTime / 1000)))
      {
        LOGF("Avg. TickRate: %d/s\n", (((float)tickSum / (float)tickTime) / (float)1000));
        tickSum = 0;
        tickCount = 0;
      }
  }

  uint8_t TickManager::GetTickTime()
  { 
    return (TickTime / 1000); 
  }

  void TickManager:::SetTickTime(uint8_t TickTime)
  { 
    tickTime = TickTime * 1000; 
  }
}
#endif // AMBIENCE_CORE
