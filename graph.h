#ifndef GRAPH_H
#define GRAPH_H

#include <LiquidCrystal_I2C.h>

#define CHAR_W 5
#define CHAR_H 8
#define CUSTOM_CHARS 8

const int MAX_HISTORY_POINTS = 20;

enum HistoryValue {
  CPU = 0,
  RAM,
  NET_I,
  NET_O,
  HDD_R,
  HDD_W,
  HistoryItemsCount
};
unsigned long History[HistoryItemsCount][MAX_HISTORY_POINTS];

void clearHistory() {
  for (uint8_t i = 0; i < MAX_HISTORY_POINTS; i++)
    for (uint8_t j = 0; j < HistoryItemsCount; j++)
      History[j][i] = 0;
}

void addToHistory(unsigned long value, HistoryValue h) {
  // Shift all history data in the row
  for (uint8_t i = MAX_HISTORY_POINTS - 1; i > 0; i--)
    History[(byte)h][i] = History[(byte)h][i - 1];
  History[(byte)h][0] = value;
}

#endif
