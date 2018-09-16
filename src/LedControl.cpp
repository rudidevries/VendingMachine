#include "LedControl.h"

#define LED_STATE_SPOTLIGHT B00000001
#define LED_STATE_IDLE      B00000010

LedControl::LedControl(uint8_t dataPin, uint8_t numSpotlightColors) : _strip(22, dataPin) {
  _state = LED_STATE_IDLE;
  _timeout = 0;

  // Initialize color settings.
  _spotlightColors = new uint32_t[numSpotlightColors];
  _spotlightEndIndex = _spotlightStartIndex = 0;
  for (int i = 0; i < numSpotlightColors; i++) {
    _spotlightColors[i] = _strip.Color(255, 255, 255);
  }
  _idleColor = _strip.Color(255, 255, 255);

  _strip.begin();
  _strip.show();
}

void LedControl::setIdleColor(uint8_t r, uint8_t g, uint8_t b) {
  _idleColor = _strip.Color(r, g, b);
}

void LedControl::setSpotlightColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
  _spotlightColors[index] = _strip.Color(r, g, b);
}

// Fill the dots one after the other with a color
void LedControl::_colorWipe(uint32_t s, uint32_t e, uint32_t c, uint8_t wait) {
  for(uint16_t i=s; i < e; i++) {
    _strip.setPixelColor(i, c);
    _strip.show();
    delay(wait);
  }
}

void LedControl::_instantColor(uint32_t c) {
  for(uint16_t i=0; i < _strip.numPixels(); i++) {
    _strip.setPixelColor(i, c);
  }
  _strip.show();
}

void LedControl::spotlight(uint8_t compartment, uint8_t colorIndex, unsigned long timeoutMillis) {
  _state = LED_STATE_SPOTLIGHT;
  _spotlightColorIndex = colorIndex;

  uint8_t indexes[4] = {15, 19, 8, 12};
  _spotlightStartIndex = indexes[compartment - 1];
  _spotlightEndIndex = _spotlightStartIndex + 3;
  _timeout = millis() + timeoutMillis;
}

void LedControl::idle() {
  _state = LED_STATE_IDLE;
}

void LedControl::loop() {
  if (_state != 0) {
    if (millis() < _timeout) {
      if (_state & LED_STATE_SPOTLIGHT) {
        _state &= ~LED_STATE_SPOTLIGHT;
        _instantColor(_strip.Color(0, 0, 0));
        _colorWipe(_spotlightStartIndex, _spotlightEndIndex, _spotlightColors[_spotlightColorIndex], 50);
        _state |= LED_STATE_IDLE;
        Serial.println("LedControl Spotlight");
      }
    }
    else {
      if (_state & LED_STATE_IDLE) {
        _state &= ~LED_STATE_IDLE;
        _instantColor(_idleColor);
        Serial.println("LedControl Idle");
      }
      _state = 0;
    }
  }
}
