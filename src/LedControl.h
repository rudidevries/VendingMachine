#ifndef LedControl_H
  #define LedControl_H

  #include <Adafruit_NeoPixel.h>

  class LedControl {
  public:
    LedControl(uint8_t dataPin, uint8_t numSpotlightColors);

    void setIdleColor(uint8_t r, uint8_t g, uint8_t b);
    void setSpotlightColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b);

    void spotlight(uint8_t compartment, uint8_t colorIndex, unsigned long timeoutMillis);
    void idle();

    void loop();
  private:
    byte _state;
    unsigned long _timeout;
    Adafruit_NeoPixel _strip;

    uint16_t _spotlightStartIndex;
    uint16_t _spotlightEndIndex;
    uint8_t _spotlightColorIndex;

    uint32_t *_spotlightColors;
    uint32_t _idleColor;

    void _colorWipe(uint32_t s, uint32_t e, uint32_t c, uint8_t wait);
    void _instantColor(uint32_t c);
  };
#endif
