#ifndef THEBUTTON_H
#define THEBUTTON_H

#include <Bounce2.h>            // https://github.com/thomasfredericks/Bounce2

struct TickEvent {
  const uint8_t tick;
};

typedef void (*cb_click)();
typedef void (*cb_tick)(uint8_t);
typedef void (*cb_tickuse)(uint8_t);

class TheButton {
  public:
    TheButton() { }

    void Setup(uint8_t pin);
    void Loop();

    void SetClickCallback(cb_click clickcb) {
      _clickCallback = clickcb;
    }

    void SetTickCallback(cb_tick tickcb) {
      _tickCallback = tickcb;
    }

    void SetTickUseCallback(cb_tickuse tickusecb) {
      _tickUseCallback = tickusecb;
    }

  private:
    Bounce _Debouncer = Bounce();
    bool _pressed = false;
    unsigned long _pressedAt = 0;   // The time when button was pressed

    uint8_t _ticks = 0;             // How many ticks were generated
    unsigned long _tickAt = 0;      // The time when the last tick was generated
    uint16_t _tickInterval = 1000;  // Count new tick every 1000 ms

    cb_click _clickCallback;
    cb_tick _tickCallback;
    cb_tickuse _tickUseCallback;
};

#endif
