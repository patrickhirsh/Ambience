#ifndef AMBIENCE_NODE_LED
#define AMBIENCE_NODE_LED

#include "AmbienceNodeCore.h"

CRGB leds[NUM_LEDS];
CHSV dynamicHue = CHSV(0, 255, 255);

void InitLEDs() 
{
  FastLED.clear();
  FastLED.show();
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void UpdateLEDs() 
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = dynamicHue;
  }

  delay(50);
  
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

#endif
