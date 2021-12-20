#ifndef AMBIENCE_CORE
#define AMBIENCE_CORE


// ==================== Globals ==================== //

// AmbienceNode Software Version
#define VERSION "1.3"

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

// Default minimum tick time in miliseconds (17ms ~ 60hz, 8ms = 125hz, 5ms = 200hz)
#define TICKTIME 5

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

// Maximum number of LED modes supported
#define MAX_MODES 64

// WiFi credentials... TODO: don't store in plain text
#define WIFI_SSID "HNET2"
#define WIFI_PASS "Flask!Deranged1!Oasis!Jaws"


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


// Number of LEDs
#define NUM_LEDS 100

// ESP libraries
#if USE_OTA
#include <ArduinoOTA.h>     // Requires ArduinoOTA by Juraj Andrassy
#endif // USE_OTA
#if USE_NETWORK
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#endif // USE_NETWORK

// Standard library
#include <limits>

// NeoPixel
#include <Adafruit_NeoPixel.h>

// JSON
#include <ArduinoJson.h>


namespace Ambience
{
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


  // ==================== Hardware ==================== //

  void InitHardware()
  {
    pinMode(EXTERN_LED, OUTPUT);
    pinMode(BUILTIN_LED, OUTPUT);

    if (!SPIFFS.begin(true))
    {
      LOG("failed to mount SPIFFS file system");
      ESP.restart();
    }
    else
    {
      LOG("SPIFFS file system mounted");
    }

    // 3 quick LED flashes indicate boot sequence start
    for (int i = 0; i < 3; i++)
    {
      PULL_LED_INDICATOR(HIGH);
      delay(200);
      PULL_LED_INDICATOR(LOW);
      delay(200);
    }
  }


  // ==================== WiFi ==================== //

  void InitWiFi() 
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

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


  // ==================== Tick Manager ==================== //
  class TickManager
  {
    public:
      static TickManager&     GetInstance();
      TickManager             (TickManager const&) = delete;                        // don't implement
      void operator=          (TickManager const&) = delete;                        // don't implement
      uint8_t                 GetTickTime();                                        // returns the current ticktime (in milliseconds)
      void                    SetTickTime(uint8_t TickTime);                        // sets a ticktime (in milliseconds)
      float                   GetAvgTickTime() { return timer.GetAvgTickTime(); }   // average tick time over the moving historical average in milliseconds
      float                   GetAvgTickRate() { return timer.GetAvgTickRate(); }   // average ticks / sec over the moving historical average
      void                    Tick();                                               // execute exactly once per tick in the core loop

    private:
      TickManager             ();
      ~TickManager            () {}
      uint32_t                tickTime;                                             // minimum time per tick (in microseconds)

#if DEBUG
      uint16_t                tickCounter = 0;                                      // for performance stats logging
#endif


      /* Simple timer that tracks time passed between invocations of its "Delta()" method.
      This timer also maintains a moving historical average of the delta values it yields over time. */
      class Timer
      {
        private:
          /* Very simple FIFO circular buffer used to keep a moving historical average on tick times in the TickManager */
          class TickTimeBuffer
          {
            private:
              // adjust this for greater accuracy when averaging the buffer at the cost of memory
              static const uint32_t CAPACITY = 200;

            public:
              TickTimeBuffer   () { Clear(); }                          // initializes a buffer of size CAPACITY to 0 values
              uint16_t          Capacity() const { return CAPACITY; }   // returns the capacity of the buffer

              /* returns an average of all the buffer values */
              uint32_t GetBufferAverage()
              {
                uint32_t sum = 0U;
                for (uint16_t i = 0; i < Capacity(); i++) 
                {
                  sum += buffer[i];
                }
                return sum / CAPACITY;
              }

              /* Resets the head and buffer to all 0 values */ 
              void Clear() 
              { 
                for (uint16_t i = 0; i < Capacity(); i++) 
                { 
                  buffer[i] = 0U; 
                }
                head = 0U; 
              }

              /* Pushs a new value to the buffer. If CAPACITY has been reached, the oldest value will be overriden. */ 
              void Push(uint32_t& item) 
              { 
                buffer[head] = item; 
                head++; 
                if (head >= CAPACITY) { head = 0U; }
              }

            private:
              uint32_t          buffer[CAPACITY];
              uint16_t          head = 0U;
          };
          TickTimeBuffer buffer;

        public:
          /* Default Constructor */
          Timer() 
          { 
            gettimeofday(&tv, NULL);
            lastTimestamp = (uint64_t)tv.tv_sec * 1000000L + (uint64_t)tv.tv_usec; 
          }

          /* captures the total time (in microseconds) of the current tick and pushes it to the moving average buffer.
          Resets currentTimestamp to track the next tick time. */
          uint32_t CaptureDelta()
          {
            gettimeofday(&tv, NULL);
            uint32_t currentTimestamp = (uint32_t)tv.tv_sec * 1000000L + (uint32_t)tv.tv_usec;
            uint32_t delta = lastTimestamp > currentTimestamp ?
              (std::numeric_limits<uint32_t>::max() - lastTimestamp) + currentTimestamp :   // check for overflow!
              currentTimestamp - lastTimestamp;
            buffer.Push(delta);
            return delta;
          }

          /* Resets lastTimestamp to the current time, basing future delta computations off this new timestamp */
          void ResetDelta()
          {
            gettimeofday(&tv, NULL);
            lastTimestamp = (uint32_t)tv.tv_sec * 1000000L + (uint32_t)tv.tv_usec;
          }

          /* returns the average tick time over the moving historical average in milliseconds */
          float GetAvgTickTime()
          {
            // buffer average (microseconds) to milliseconds
            return (float)buffer.GetBufferAverage() / (float)1000;
          }

          /* returns the current ticks per second */
          float GetAvgTickRate()
          {
            float realTickTime = GetAvgTickTime() < TickManager::GetInstance().GetTickTime() ? 
              TickManager::GetInstance().GetTickTime() : 
              GetAvgTickTime();
            return (float)1000 / realTickTime;      // ticks (number of "realTickTimes") per second
          }
        
        private:
          uint64_t lastTimestamp;
          struct timeval tv;
      };
      Timer timer;
  };


  TickManager& TickManager::GetInstance()
  {
    static TickManager singleton;
    return singleton;
  }


  TickManager::TickManager()
  {
    tickTime = TICKTIME * 1000;
  }


  uint8_t TickManager::GetTickTime()
  { 
    // integer precision is sufficient since TickTime can only be set with uint8_t
    return (tickTime / 1000); 
  }


  void TickManager::SetTickTime(uint8_t TickTime)
  { 
    tickTime = TickTime * 1000; 
  }


  void TickManager::Tick()
  {
    uint32_t tickDelta = timer.CaptureDelta();
    if (tickDelta < tickTime)
    {
      delayMicroseconds(tickTime - tickDelta);
    }
    timer.ResetDelta();

#if DEBUG
    tickCounter++;
    if (tickCounter >= ((1000 / (tickTime / 1000)) * 5))   // print every 5 seconds
    {
      LOGF("[Tick: %.2fms/", timer.GetAvgTickTime());
      LOGF("%.2fms", (float)tickTime / (float)1000);
      LOGF(" (%.2f%% of target)] ", ((float)(tickTime / 1000) / (float)(timer.GetAvgTickTime())) * 100);
      LOGF("[TickRate: %.1f]\n", timer.GetAvgTickRate());
      tickCounter = 0;
    }
#endif
  }
}
#endif // AMBIENCE_CORE
