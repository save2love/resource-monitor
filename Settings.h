#ifndef SETTINGS_H
#define SETTINGS_H

#define SCR_DEFAULT                 0
#define SCR_CPU_RAM                 1
#define SCR_CPU_DETAILED            2
#define SCR_NET_HDD                 3
#define SCR_NET_HDD_DETAILED        4
#define SCR_NET_DETAILED            5
#define SCR_HDD_DETAILED            6
//-----------------------------------
#define SCREENS_COUNT               7

class Settings {
  public:
    uint8_t Screen = SCR_DEFAULT;
    bool IsBacklight = true;
    bool AutoChange = true;
  
    Settings() {};
    void Load();
    void Save();

};

#endif
