#include "TheButton.h"

void TheButton::Setup(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);
  _Debouncer.attach(pin);
  _Debouncer.interval(5);
}

void TheButton::Loop() {
  unsigned long t = millis();
  if ( _Debouncer.update() ) {
    // Get the update value
    int value = _Debouncer.read();
    if ( value == HIGH) { // Release
      if (_ticks > 0) { // Long press
        if (_tickUseCallback)
          (*_tickUseCallback)(_ticks);
      }
      else { // short press
        if (_clickCallback) {
          (*_clickCallback)();
        }
      }
      _pressed = false;
      _ticks = 0;
    } else { // Press
      _pressed = true;
      _pressedAt = t;
      _tickAt = t;
    }
  }

  if  ( _pressed ) { // Long press
    if ( t - _tickAt >= _tickInterval ) {
      _tickAt = t;
      _ticks++;

      if (_tickCallback)
        (*_tickCallback)(_ticks);
    }
  }
}
