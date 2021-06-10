#ifndef AMBIENCE_LED
#define AMBIENCE_LED

#include "Core.h"


namespace Ambience
{ 

  class LEDStrip
  {

    // ==================== LED Mode ==================== //

    public:
      enum Mode
      {
        Color,
      };
    
    private:
      class LEDMode
      {
        public:
          virtual void Update(CRGB (&leds)[NUM_LEDS], const CHSV color1, const CHSV color2, const CHSV color3) {};
      };


      // ==================== LEDMode_Color ==================== //

      class LEDMode_Color : public LEDMode 
      {
        void Update(CRGB (&leds)[NUM_LEDS], const CHSV color1, const CHSV color2, const CHSV color3)
        {
          for (int i = 0; i < NUM_LEDS; i++)
          {
            leds[i] = color1;
          }
        }
      };


    // ==================== LED Strip ==================== //

    public:
      LEDStrip      ();
      ~LEDStrip     () {}
      void          Update();
      void          SetMode(Mode mode);
      Mode          GetMode();
      void          SetBrightness(float brightnessScalar);
      float         GetBrightness();
      void          SetActive(bool isActive);
      bool          GetActive();

      CHSV          Color1;
      CHSV          Color2;
      CHSV          Color3;

    private:
      CRGB          leds[NUM_LEDS];
      bool          active;
      Mode          currentMode;
      LEDMode       currentLEDMode;
      float         brightness;

  };

  LEDStrip::LEDStrip()
  {
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.show();

    active = true;
    brightness = 1.0;
    Color1 = CHSV(0, 0, 255);
    Color2 = CHSV(0, 0, 255);
    Color3 = CHSV(0, 0, 255);
    currentMode = Mode::Color;
    currentLEDMode = LEDMode_Color();
    LOG("LEDs Initialized");
  }

  void LEDStrip::Update()
  {
    if (active)
    {
      currentLEDMode.Update(
        leds, 
        CHSV(Color1.h, Color1.s, Color1.v * brightness),
        CHSV(Color2.h, Color2.s, Color2.v * brightness), 
        CHSV(Color3.h, Color3.s, Color3.v * brightness));

      // TODO: Not This (LED Modes should be updating these...)
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = Color1;
      }
      FastLED.show();
    }
  }

  void LEDStrip::SetMode(LEDStrip::Mode mode)
  {
    switch(mode)
    {
      case LEDStrip::Mode::Color:
        currentLEDMode = LEDMode_Color();
        break;
      default:
        return;
    }
    currentMode = mode;
  }

  LEDStrip::Mode LEDStrip::GetMode()
  {
    return currentMode;
  }

  void LEDStrip::SetBrightness(float brightnessScalar)
  {
    // chop out-of-range values
    if (brightnessScalar > 1.0 ) { brightnessScalar = 1.0; }
    if (brightnessScalar < 0.0 ) { brightnessScalar = 0.0; }
    brightness = brightnessScalar;
  }

  float LEDStrip::GetBrightness()
  {
    return brightness;
  }

  void LEDStrip::SetActive(bool isActive)
  {
    if (!isActive)
    {
      FastLED.clear();
      FastLED.show();
    }
    active = isActive;
  }

  bool LEDStrip::GetActive()
  {
    return active;
  }
}
#endif
