#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
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
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>
#include <math.h>

#define DATA_PIN 12
#define NUM_LEDS 10
#define BAUD_RATE 9600

CRGB leds[NUM_LEDS];

void setup() 
{
  Serial.begin(BAUD_RATE);
  
  FastLED.clear();
  FastLED.show();
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() 
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(255,0,0);
  }
  FastLED.show();
  delay(500);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0,0,0);
  }
  FastLED.show();
  delay(500);

  Serial.print("Hello!\n");
}
