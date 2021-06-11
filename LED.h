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
          virtual ~LEDMode() = 0;
          virtual void Update(CHSV* buffer, const CHSV &color1, const CHSV &color2, const CHSV &color3) = 0;
      };


      // ==================== LEDMode_Color ==================== //

      class LEDMode_Color : public LEDMode 
      {
        ~LEDMode_Color() {}
        void Update(CHSV* buffer, const CHSV &color1, const CHSV &color2, const CHSV &color3)
        {
          for (int i = 0; i < NUM_LEDS; i++)
          {
            buffer[i] = color1;
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
      CHSV          buffer[NUM_LEDS];
      bool          active;
      Mode          currentMode;
      LEDMode*      currentLEDMode;
      float         brightness;

  };

  LEDStrip::LEDMode::~LEDMode() {}

  LEDStrip::LEDStrip()
  {
    // bind led array to FastLED library
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.show();

    // initialize buffer
    for (int i = 0; i < NUM_LEDS; i++)
    {
      buffer[i] = CHSV(0, 0, 0);
    }

    // set defaults
    active = true;
    brightness = 1.0;
    Color1 = CHSV(0, 0, 255);
    Color2 = CHSV(0, 0, 255);
    Color3 = CHSV(0, 0, 255);
    currentMode = Mode::Color;
    currentLEDMode = new LEDMode_Color();
    LOG("LEDs Initialized");
  }


  void LEDStrip::Update()
  {
    if (active)
    {
      // retrieve an updated buffer based on current mode and colors
      currentLEDMode->Update(buffer, Color1, Color2, Color3);

      // enforce strip settings / post processing on led array write
      for (int i = 0; i < NUM_LEDS; i++)
      {
        uint8_t h = buffer[i].h;
        uint8_t s = buffer[i].s;
        uint8_t v = buffer[i].v * brightness;
        leds[i] = CHSV(h, s, v);
      }
      FastLED.show();
    }
  }


  void LEDStrip::SetMode(LEDStrip::Mode mode)
  {
    LEDStrip::LEDMode* oldLEDMode = currentLEDMode;
    switch(mode)

    {
      // *** Mode -> LEDMode Mapping *** //

      case LEDStrip::Mode::Color:
        currentLEDMode = new LEDMode_Color();
        break;

      default:
        return;
    }

    delete oldLEDMode;
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
