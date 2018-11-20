#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H

#include "Settings.h"
#include "helper.h"
#include "graph.h"
#include "TheButton.h"
#include <LiquidCrystal_I2C.h> // https://github.com/mrkaleArduinoLib/LiquidCrystal_I2C

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
