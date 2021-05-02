#ifndef AMBIENCE_NODE_LED
#define AMBIENCE_NODE_LED

#include "AmbienceNodeCore.h"


namespace AmbienceNodeLED
{
  class LEDMode
  {
    public:
      virtual void Update();
  };

  namespace
  {
    LEDMode* currentMode;
    CRGB leds[NUM_LEDS];

    // ==================== Color ==================== //

    class LEDModeColor : public LEDMode
    {
      public:
        LEDModeColor();
        void Update();
        void SetColor(CHSV c) { color = c; }
        CHSV GetColor() { return color; }

      private:
        CHSV color;
    };

    LEDModeColor::LEDModeColor()
    {
      FastLED.clear();
      CHSV color = CHSV(0, 0, 0);
    }

    void LEDModeColor::Update()
    {
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = color;
      }
    }

    LEDModeColor* M_Color;

  }

  void Init() 
  {
    // Init LED Modes
    M_Color = new LEDModeColor();
    currentMode = M_Color;

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.show();
  }

  void Update() 
  {
    currentMode->Update();
    FastLED.show();
  }

  void SetMode(LEDMode* mode)
  {
    currentMode = mode;
  }

  LEDMode* GetMode()
  {
    return currentMode;
  }
}
#endif
