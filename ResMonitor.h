#ifndef RESMONITOR_H
#define RESMONITOR_H

#include "Settings.h"

#define BUTTON_PIN 2

long screen_was_changed_at = 0;
//uint8_t current_screen = SCR_DEFAULT; // Current screen
bool forceChange = false;

// The machine states
enum States {
  ST_Default  = 0,
  ST_Settings = 1
} _State = ST_Default;

#endif
