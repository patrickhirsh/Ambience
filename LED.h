#ifndef AMBIENCE_LED
#define AMBIENCE_LED

#include "Core.h"


namespace Ambience
{ 
  class LEDStrip
  {
    public:
      LEDStrip            ();
      ~LEDStrip           () {}
      void                Update();

      bool                SetMode(String mode);
      String              GetMode();
      void                SetBrightness(uint8_t brightness);
      uint8_t             GetBrightness();
      void                SetActive(bool isActive);
      bool                GetActive();


      /*
      TODO:
        - Store colors locally in the form of HSV (or HSVW?) with "Color"
        - Write a conversion method that converts and packs the HSV (or HSVW) value into Neopixel's packed WRGB uint32_t
          - Will this be too computationally expensive to perform on EVERY pixel EVERY tick?
          - if so.. consider storing the raw buffer in uint32_t and providing an HSV conversion function for classes that utilize it?
            - increased complexity for writing modes
            - possibly a pre-optimization step that is uneccesary

        - Also consider: A DEBUG-ONLY timing system for tracking frame time
      */
      class Color
      {
        public:
          uint8_t   H = 0;
          uint8_t   S = 0;
          uint8_t   V = 0;
          uint8_t   W = 0;

          Color() {}
          Color(uint8_t h, uint8_t s, uint8_t v, uint8_t w = 0) 
          { 
            H = h; 
            S = s; 
            V = v; 
            W = w; 
          }

          /* Returns a packed 4-byte "WRGB" value*/
          uint32_t WRGB()
          {
            // Test - W=255, R=0, G=0, B=0
            return 4278190080;
          }
      };

      Color               Color1;
      Color               Color2;
      Color               Color3;


      // ==================== Modes ==================== //

      /*
      Objects that derive from "Mode" represent a lighting mode that can mutate the state of the LEDs (buffer) each tick.
      Modes should utilize the passed color palette when appropriate and consider NUM_LEDS when writing to the buffer.
      "Update()" will be invoked once per tick and "buffer" will always contain NUM_LEDS elements.

      To declare and implement a new mode:
          - implement your class derriving from the LEDStrip::Mode class below
          - add a case for your mode in LEDStrip::SetMode so it can be used at runtime
      */
      class Mode
      {
        public:
          virtual ~Mode() = 0;
          virtual void Update(Color* buffer, const Color &color1, const Color &color2, const Color &color3) = 0;
      };


      // ==================== M_Color ==================== //

      class M_Color : public Mode 
      {
        ~M_Color() {}
        void Update(Color* buffer, const Color &color1, const Color &color2, const Color &color3)
        {
          for (int i = 0; i < NUM_LEDS; i++)
          {
            buffer[i] = color1;
          }
        }
      };


      // ==================== M_Rainbow ==================== //

    /* TODO: Hue is broken... Fix that

      class M_Rainbow : public Mode 
      {
        int hue = 0;
        ~M_Rainbow() {}
        void Update(Color* buffer, const Color &color1, const Color &color2, const Color &color3)
        {
          hue++;
          if (hue > 255) { hue = 0;}
          for (int i = 0; i < NUM_LEDS; i++)
          {
            buffer[i] = CHSV(hue, 255, 255);
          }
        }
      };
      */

    private:
      Adafruit_NeoPixel   leds;

      Color               buffer[NUM_LEDS];   // raw led buffer (no post-processing)
      bool                active;             // should the leds be turned on?
      float               brightness;         // 0.0f - 1.0f brightness value
      Mode*               mode;               // current mode
      String              modeName;           // name of the current mode
  };

  LEDStrip::Mode::~Mode() {}


  LEDStrip::LEDStrip()
  {
    leds = Adafruit_NeoPixel((uint16_t)NUM_LEDS, (uint16_t)DATA_PIN, (neoPixelType)NEOPIXEL_FLAGS);
    leds.begin();
    leds.clear();
    leds.show();

    // initialize buffer
    for (int i = 0; i < NUM_LEDS; i++)
    {
      buffer[i] = Color();
    }

    // set defaults
    active = true;
    Color1 = Color();
    Color2 = Color();
    Color3 = Color();
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

      // apply post processing step on buffer, then write to strip
      for (int i = 0; i < NUM_LEDS; i++)
      {
        // Post-processing (transitions/fades/etc.) apply here..

        leds.setPixelColor(i, buffer[i].WRGB());
      }
      leds.show();
    }
  }


  bool LEDStrip::SetMode(String Name)
  {
    // This maybe isn't the best way to declare a definitive list of supported modes... Might revisit this
    if (Name == "Color") { delete mode; mode = new M_Color(); modeName = Name; return true; }
    //if (Name == "Rainbow") { delete mode; mode = new M_Rainbow(); modeName = Name; return true; }
    return false;
  }


  String LEDStrip::GetMode()
  {
    return modeName;
  }


  void LEDStrip::SetBrightness(uint8_t brightness)
  {
    leds.setBrightness(brightness);
  }


  uint8_t LEDStrip::GetBrightness()
  {
    return leds.getBrightness();
  }


  void LEDStrip::SetActive(bool isActive)
  {
    if (!isActive)
    {
      leds.clear();
      leds.show();
    }
    active = isActive;
  }


  bool LEDStrip::GetActive()
  {
    return active;
  }
}
#endif
