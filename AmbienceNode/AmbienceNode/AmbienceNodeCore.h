#ifndef AMBIENCE_NODE_CORE
#define AMBIENCE_NODE_CORE

// Standard Library
#include <math.h>

// ESP Library
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
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


// ==================== Globals ==================== //

// Enables logging over serial port 115200
#define DEBUG 1

// Serial BAUD rate
#define BAUDRATE 115200


// ==================== Logging ==================== //

void InitLogging() {
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

void InitWiFi() {

  // WiFi credentials... TODO: don't store in plain text
  const char* ssid = "";
  const char* pass = "";

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

#endif
