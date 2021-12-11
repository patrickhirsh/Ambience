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

      class Color
      {
        public:
          uint16_t   H = 0;
          uint8_t   S = 0;
          uint8_t   V = 0;
          uint8_t   W = 0;

          Color() {}
          Color(uint16_t h, uint8_t s, uint8_t v, uint8_t w = 0) 
          { 
            H = h; 
            S = s; 
            V = v; 
            W = w; 
          }

          /* 
          This is a slightly modified version of Neopixel's ColorHSV() method
          Found in Adafruit_NeoPixel.cpp - with the inclusion of our packed
          white channel. 
          
          Returns a packed 4-byte "WRGB" value. */
          uint32_t WRGB()
          {
            uint8_t r, g, b;

            // The 8-bit RGB hexcone (256 values each for red, green, blue) really 
            // only allows for 1530 distinct hues (not 1536, more on that below), 
            // but the full unsigned 16-bit type was chosen for hue so that one's 
            // code can easily handle a contiguous color wheel by allowing hue to 
            // roll over in either direction.
            uint16_t h = (H * 1530L + 32768) / 65536;

            // Because red is centered on the rollover point (the +32768 above,
            // essentially a fixed-point +0.5), the above actually yields 0 to 1530,
            // where 0 and 1530 would yield the same thing. Rather than apply a
            // costly modulo operator, 1530 is handled as a special case below.

            // So you'd think that the color "hexcone" (the thing that ramps from
            // pure red, to pure yellow, to pure green and so forth back to red,
            // yielding six slices), and with each color component having 256
            // possible values (0-255), might have 1536 possible items (6*256),
            // but in reality there's 1530. This is because the last element in
            // each 256-element slice is equal to the first element of the next
            // slice, and keeping those in there this would create small
            // discontinuities in the color wheel. So the last element of each
            // slice is dropped...we regard only elements 0-254, with item 255
            // being picked up as element 0 of the next slice. Like this:
            // Red to not-quite-pure-yellow is:        255,   0, 0 to 255, 254,   0
            // Pure yellow to not-quite-pure-green is: 255, 255, 0 to   1, 255,   0
            // Pure green to not-quite-pure-cyan is:     0, 255, 0 to   0, 255, 254
            // and so forth. Hence, 1530 distinct hues (0 to 1529), and hence why
            // the constants below are not the multiples of 256 you might expect.

            // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
            if (h < 510) { // Red to Green-1
              b = 0;
              if (h < 255) { //   Red to Yellow-1
                r = 255;
                g = h;       //     g = 0 to 254
              } else {         //   Yellow to Green-1
                r = 510 - h; //     r = 255 to 1
                g = 255;
              }
            } else if (h < 1020) { // Green to Blue-1
              r = 0;
              if (h < 765) { //   Green to Cyan-1
                g = 255;
                b = h - 510;  //     b = 0 to 254
              } else {          //   Cyan to Blue-1
                g = 1020 - h; //     g = 255 to 1
                b = 255;
              }
            } else if (h < 1530) { // Blue to Red-1
              g = 0;
              if (h < 1275) { //   Blue to Magenta-1
                r = h - 1020; //     r = 0 to 254
                b = 255;
              } else { //   Magenta to Red-1
                r = 255;
                b = 1530 - h; //     b = 255 to 1
              }
            } else { // Last 0.5 Red (quicker than % operator)
              r = 255;
              g = b = 0;
            }

            // Apply saturation and value to R,G,B, pack into 32-bit result with white:
            uint32_t v1 = 1 + V;  // 1 to 256; allows >>8 instead of /255
            uint16_t s1 = 1 + S;  // 1 to 256; same reason
            uint8_t s2 = 255 - S; // 255 to 0
            return  W << 24 | 
                  ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
                  (((((g * s1) >> 8) + s2) * v1) & 0xff00) |
                  (((((b * s1) >> 8) + s2) * v1) >> 8);
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
    leds = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEOPIXEL_FLAGS);
    leds.begin();
    leds.clear();
    leds.show();

    // set defaults
    active = true;
    Color1 = Color();
    Color2 = Color();
    Color3 = Color();
    mode = new M_Color();
    modeName = "Color";

    // initialize buffer
    for (int i = 0; i < NUM_LEDS; i++)
    {
      buffer[i] = Color(0, 0, 0, 0);
    }

    // initialize strip with buffer
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds.setPixelColor(i, buffer[i].WRGB());
    }

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
