#include <LiquidCrystal_I2C.h>  // https://github.com/marcoschwartz/LiquidCrystal_I2C
#include "helper.h"
#include "graph.h"
#include "TheButton.h"
#include "ResMonitor.h"

LiquidCrystal_I2C lcd(0x27, 20, 4); // Check I2C address of LCD, normally 0x27 or 0x3F

bool need_to_clear = true;
long last_data_was_received_at = 0;
bool new_data_ready = false;

const uint8_t max_packet_size = 16; // Max allowed packet size
uint8_t packet[max_packet_size];    // Incoming packet
uint8_t pi = 0;                     // Packet byte index

uint8_t type_of_data = 0; // Type of incoming data (1 - CPU, 2 - RAM, etc.)

uint16_t cpu_p = 0; // CPU Percent
uint16_t ram_p = 0; // MEM Percent
unsigned long ram_u = 0; // Used memory
unsigned long ram_f = 0; // Free memory
unsigned long net_i = 0; // Network in trafic
unsigned long net_o = 0; // Network out trafic
unsigned long hdd_r = 0; // Disk read
unsigned long hdd_w = 0; // Disk write

TheButton theButton;
Settings settings;

void setup()  {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  theButton.Setup(BUTTON_PIN);
  theButton.SetClickCallback(btnClick);
  theButton.SetTickCallback(btnTick);
  theButton.SetTickUseCallback(btnTickUse);

  settings.Load();

  lcd.init(); // initialize the lcd
  if (settings.IsBacklight)
    lcd.backlight();
  else
    lcd.noBacklight();
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("--- Awaiting ---");
  lcd.setCursor(2, 2);
  lcd.print("-- connection --");
  randomSeed(analogRead(A0));
  clearHistory();

  delay(500);
}

void loop() {
  theButton.Loop();

  switch (_State) {
    case ST_Default:
    default:

      receivingLoop();

      if (new_data_ready) {
        if ((settings.AutoChange && (millis() - screen_was_changed_at) >= getScreenShowTime() * 1000) || forceChange) {
          // Change screen if new data was received and previos screen was showed more than 10 sec
          settings.Screen++;
          if (settings.Screen >= SCREENS_COUNT)
            settings.Screen = 0;
          screen_was_changed_at = millis();
          need_to_clear = true;
          forceChange = false;
        }

        clearIfNecessary();
        showNewData();

        new_data_ready = false;
      }
      break;

    case ST_Settings:

      break;
  }

  showAwaitinigDataMsgIfNecessary();
}

void clearIfNecessary() { // Clear all display if it is necessary
  if (!need_to_clear) return;
  //lcd.clear();
  need_to_clear = false;
}

void showAwaitinigDataMsgIfNecessary() {
  if (!need_to_clear && (millis() - last_data_was_received_at) >= 10 * 1000)
  {
    lcd.clear();
    lcd.setCursor(2, 1);
    lcd.print("--- Awaiting ---");
    lcd.setCursor(2, 2);
    lcd.print("--- data ... ---");
    clearHistory();
    need_to_clear = true; // Need to clear next time when new data will be available
  }
}

void receivingLoop() {
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    last_data_was_received_at = millis();

    switch (inChar) {
      case '[': // Start new data packet
        pi = 0;
        delay(10); // wait a bit to serial
        type_of_data = Serial.read();
        break;

      case '\n': // End of data packet
        switch (type_of_data) {
          case 1: cpu_p = getUInt(packet); addToHistory(cpu_p, CPU); break;
          case 2: ram_p = getUInt(packet); addToHistory(ram_p, RAM); break;
          // Memory usage, network traffic, disk read bytes and others byte values transfers in kilobytes.
          // Value in unsigned long (4-bytes) can be in range between 0 to 4,294,967,295.
          // It means you can store the value from 0 bytes to 4Gb but it is not enough.
          // Reducing the accuracy to kilobytes, the maximum value increases to 4 Tb.
          case 3: ram_u = getLong(packet); break;
          case 4: ram_f = getLong(packet); break;
          case 5: net_i = getLong(packet); addToHistory(net_i, NET_I); break;
          case 6: net_o = getLong(packet); addToHistory(net_o, NET_O); break;
          case 7: hdd_r = getLong(packet); addToHistory(hdd_r, HDD_R); break;
          case 8: hdd_w = getLong(packet); addToHistory(hdd_w, HDD_W); break;
        }
        new_data_ready = true;
        break;

      default: // Collect data in packet byte array
        packet[pi] = (byte)inChar;
        pi++;
        if (pi >= max_packet_size) pi = max_packet_size - 1;
        break;
    }
  }
}

void showCPUandRAM(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print("CPU" + strPad(String(cpu_p / 10.0, 1), 5) + '%');
  lcd.setCursor(11, row);
  lcd.print("RAM" + strPad(String(ram_p / 10.0, 1), 5) + '%');
}

void showRAMUtilization(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print("RAM");
  lcd.print(strPad(convertSize(ram_u), 8));
  lcd.print(char(0x7C));
  lcd.print(strPad(convertSize(ram_f), 8));
}

void showNetworkActivity(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print("NET");
  lcd.print(strPad(convertSize(net_i), 8));
  lcd.print(char(0x7C));
  lcd.print(strPad(convertSize(net_o), 8));
}

void showDiskActivity(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print("HDD");
  lcd.print(strPad(convertSize(hdd_r), 8));
  lcd.print(char(0x7C));
  lcd.print(strPad(convertSize(hdd_w), 8));
}

uint8_t getScreenShowTime() {
  switch (settings.Screen) {
    default:
    case SCR_DEFAULT: return 30;
    case SCR_CPU_RAM: return 15;
    case SCR_NET_HDD: return 20;
  }
}

void showNewData() {
  switch (settings.Screen) {
    default:
    case SCR_DEFAULT:
      showCPUandRAM(0);
      showRAMUtilization(1);
      showNetworkActivity(2);
      showDiskActivity(3);
      break;

    case SCR_CPU_RAM:
      graphInRow(CPU, 1, 0, 17, 2, 1000);
      graphInRow(RAM, 3, 0, 17, 2, 1000);
      lcd.setCursor(17, 0);
      lcd.print("CPU");
      lcd.setCursor(17, 1);
      if (cpu_p == 1000)
        lcd.print(strPad(String(100), 2));
      else
        lcd.print(strPad(String(cpu_p / 10.0, 0), 2) + '%');
      lcd.setCursor(15, 2);
      lcd.print("  RAM");
      lcd.setCursor(17, 3);
      if (ram_p == 1000)
        lcd.print(strPad(String(100), 2));
      else
        lcd.print(strPad(String(ram_p / 10.0, 0), 2) + '%');
      break;

    case SCR_CPU_DETAILED:
      graphInRow(CPU, 3, 0, 17, 3, 1000);
      lcd.setCursor(17, 0);
      lcd.print(" C ");
      lcd.setCursor(17, 1);
      lcd.print(" P ");
      lcd.setCursor(17, 2);
      lcd.print(" U ");
      lcd.setCursor(17, 3);
      if (cpu_p == 1000)
        lcd.print(strPad(String(100), 2));
      else
        lcd.print(strPad(String(cpu_p / 10.0, 0), 2) + '%');
      break;

    case SCR_NET_HDD:
      graphInRow(NET_I, NET_O, 1, 0, 12, 2, 500);
      graphInRow(HDD_R, HDD_W, 3, 0, 12, 2, 500);
      lcd.setCursor(17, 0);
      lcd.print("NET");
      lcd.setCursor(12, 1);
      lcd.print(strPad(convertSize(net_i + net_o), 8));
      lcd.setCursor(17, 2);
      lcd.print("HDD");
      lcd.setCursor(12, 3);
      lcd.print(strPad(convertSize(hdd_r + hdd_w), 8));
      break;

    case SCR_NET_HDD_DETAILED:
      lcd.setCursor(0, 0);
      lcd.print("U");
      lcd.setCursor(0, 1);
      lcd.print("D");
      lcd.setCursor(0, 2);
      lcd.print("R");
      lcd.setCursor(0, 3);
      lcd.print("W");

      graphInRow(NET_O, 0, 1, 11, 1, 500);
      graphInRow(NET_I, 1, 1, 11, 1, 500);
      graphInRow(HDD_R, 2, 1, 11, 1, 500);
      graphInRow(HDD_W, 3, 1, 11, 1, 500);

      lcd.setCursor(12, 0);
      lcd.print(strPad(convertSize(net_o), 8));
      lcd.setCursor(12, 1);
      lcd.print(strPad(convertSize(net_i), 8));
      lcd.setCursor(12, 2);
      lcd.print(strPad(convertSize(hdd_r), 8));
      lcd.setCursor(12, 3);
      lcd.print(strPad(convertSize(hdd_w), 8));
      break;

    case SCR_NET_DETAILED:
      graphInRow(NET_O, 1, 0, 12, 2, 500);
      graphInRow(NET_I, 3, 0, 12, 2, 500);
      lcd.setCursor(13, 0);
      lcd.print("  NET U");
      lcd.setCursor(12, 1);
      lcd.print(strPad(convertSize(net_o), 8));
      lcd.setCursor(13, 2);
      lcd.print("  NET D");
      lcd.setCursor(12, 3);
      lcd.print(strPad(convertSize(net_i), 8));
      break;

    case SCR_HDD_DETAILED:
      graphInRow(HDD_R, 1, 0, 12, 2, 500);
      graphInRow(HDD_W, 3, 0, 12, 2, 500);
      lcd.setCursor(12, 0);
      lcd.print("   HDD R");
      lcd.setCursor(12, 1);
      lcd.print(strPad(convertSize(hdd_r), 8));
      lcd.setCursor(12, 2);
      lcd.print("   HDD W");
      lcd.setCursor(12, 3);
      lcd.print(strPad(convertSize(hdd_w), 8));
      break;

  }
}

void graphInRowClear(uint8_t row, uint8_t col, uint8_t len, uint8_t high) {
  if (high > 1 && row > 0) {
    for (uint8_t r = 0; r < high - 1; r++) {
      lcd.setCursor(col, row - r - 1);
      lcd.print(strPad(String(" "), len));
    }
  }
}

// Single value graph
void graphInRow(HistoryValue h, uint8_t row, uint8_t col, uint8_t len, uint8_t high, unsigned long mapMaxValue) {
  lcd.init_bargraph(LCDI2C_VERTICAL_BAR_GRAPH);
  graphInRowClear(row, col, len, high);
  unsigned long maxmax = max(getMax(h, len), mapMaxValue);
  for (uint8_t i = 0; i < min(MAX_HISTORY_POINTS, len); i++)
    lcd.draw_vertical_graph(
      row,
      col + len - 1 - i,
      high,
      (uint8_t)map(History[h][i], 0, maxmax, 0, high * LCD_CHARACTER_VERTICAL_DOTS));
}

// Double value graph (the first item value + the second item value)
void graphInRow(HistoryValue h1, HistoryValue h2, uint8_t row, uint8_t col, uint8_t len, uint8_t high, unsigned long mapMaxValue) {
  lcd.init_bargraph(LCDI2C_VERTICAL_BAR_GRAPH);
  graphInRowClear(row, col, len, high);
  unsigned long maxmax = max(max(getMax(h1, len), getMax(h2, len)), mapMaxValue);
  for (uint8_t i = 0; i < min(MAX_HISTORY_POINTS, len); i++) {
    lcd.draw_vertical_graph(
      row,
      col + len - 1 - i,
      high,
      (uint8_t)map(History[h1][i] + History[h2][i], 0, maxmax, 0, high * LCD_CHARACTER_VERTICAL_DOTS));
  }
}

unsigned long getMax(HistoryValue h, uint8_t len) {
  unsigned long max = History[h][0];
  for (uint8_t i = 0; i < len; i++)
    max = max(max, History[h][i]);
  return max;
}

String MenuItems[] = {"Backlight", "Auto screen change", "Exit"};
#define MenuItemsCount (sizeof(MenuItems)/sizeof(String))

void btnClick()
{
  forceChange = true;
}

void btnTick(uint8_t ticks) {
  _State = ST_Settings; // Switch to Settings Mode
  byte i = (ticks - 1) % MenuItemsCount;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(i + 1);
  lcd.print("/");
  lcd.print(MenuItemsCount);

  lcd.setCursor(0, 1);

  switch (i)
  {
    case 0:
      lcd.print(MenuItems[i]);
      lcd.setCursor(0, 2);
      lcd.print(settings.IsBacklight ? F("ON->OFF") : F("OFF->ON"));
      break;

    case 1:
      lcd.print(MenuItems[i]);
      lcd.setCursor(0, 2);
      lcd.print(settings.AutoChange ? F("START->STOP") : F("STOP->START"));
      break;

    default:
      lcd.print(MenuItems[i]);
      break;
  }
}

void btnTickUse(uint8_t ticks) {
  byte i = (ticks - 1) % MenuItemsCount;
  switch (i) {
    case 0:
      settings.IsBacklight = !settings.IsBacklight;
      if (settings.IsBacklight)
        lcd.backlight();
      else
        lcd.noBacklight();
      break;

    case 1:
      settings.AutoChange = !settings.AutoChange;
      break;
  }
  
  settings.Save();
  lcd.setCursor(16, 2);
  lcd.print(F("DONE"));
  _State = ST_Default;
  need_to_clear = true;
}
