#ifndef AMBIENCE_LED
#define AMBIENCE_LED

#include "Core.h"


namespace Ambience
{ 
  class LEDStrip
  {
    private:


      // ==================== Modes ==================== //
      /*
      Modes are represented as objects derived from the "Mode" class which provide an "Update"
      function that mutates the passed "buffer" when invoked. Modes should utilize the provided 
      color palete when appropriate and work regardless of strip length.

      To support a new mode:
        - Create a derived class for the new mode which implements the required pure-virtual functions
        - Add a case for it in SetMode() to make it available as a valid mode
      */
      class Mode
      {
        public:
          virtual ~Mode() = 0;
          virtual void Update(CHSV* buffer, const CHSV &color1, const CHSV &color2, const CHSV &color3) = 0;
      };


      // ==================== M_Color ==================== //

      class M_Color : public Mode 
      {
        ~M_Color() {}
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

      bool          SetMode(String mode);
      String        GetMode();
      void          SetBrightness(float brightnessScalar);
      float         GetBrightness();
      void          SetActive(bool isActive);
      bool          GetActive();

      CHSV          Color1;
      CHSV          Color2;
      CHSV          Color3;

    private:
      CRGB          leds[NUM_LEDS];     // FastLED registered LED buffer (post-processed)
      CHSV          buffer[NUM_LEDS];   // pre post-processing buffer
      bool          active;
      float         brightness;
      Mode*         mode;
      String        modeName;
  };

  // virtual destructors
  LEDStrip::Mode::~Mode() {}


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
    mode = new M_Color();
    modeName = "Color";
    LOG("LEDs Initialized");
  }


  void LEDStrip::Update()
  {
    if (active)
    {
      // update buffer based on current mode and colors
      mode->Update(buffer, Color1, Color2, Color3);

      // apply post processing step on buffer -> FastLED-bound leds array coppy
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CHSV(buffer[i].h, buffer[i].s, buffer[i].v * brightness);
      }
      FastLED.show();
    }
  }


  bool LEDStrip::SetMode(String Name)
  {
    // This maybe isn't the best way to declare a definitive list of supported modes... Might revisit this
    if (Name = "Color") { delete mode; mode = new M_Color(); modeName = Name; return true; }
    return false;
  }


  String LEDStrip::GetMode()
  {
    return modeName;
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
