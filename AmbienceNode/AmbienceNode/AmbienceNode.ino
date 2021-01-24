#include "AmbienceNodeCore.h"

#define DATA_PIN 12
#define NUM_LEDS 10
#define BAUD_RATE 9600

CRGB leds[NUM_LEDS];
CHSV dynamicHue = CHSV(0, 255, 255);

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
    leds[i] = dynamicHue;
  }
  delay(200);
  if (dynamicHue.hue == 255)
  {
    dynamicHue.hue = 0;
  }
  else
  {
    dynamicHue.hue++;
  }
  FastLED.show();
}
