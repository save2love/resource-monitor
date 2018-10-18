#include <EEPROM.h>
#include "Settings.h"

void Settings::Load() {
  Screen = EEPROM.read(1);
  IsBacklight = EEPROM.read(2);
  AutoChange = EEPROM.read(3);
}

void Settings::Save() {
  EEPROM.write(1, Screen);
  EEPROM.write(2, IsBacklight);
  EEPROM.write(3, AutoChange);
}
