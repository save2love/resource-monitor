#ifndef HELPER_H
#define HELPER_H

unsigned long getLong(byte p[]) {
  unsigned long result = 0;
  for (byte i = 0; i < 4; i++)
    result = (result << 8) | p[i];
  return result;
}

uint16_t getUInt(byte p[]) {
  uint16_t result = 0;
  for (byte i = 0; i < 2; i++)
    result = (result << 8) | p[i];
  return result;
}

String intPad(int value, uint8_t len) {
  char format[5];
  sprintf(format, "%%%dd", len);
  char result[len + 1];
  sprintf(result, format, value);
  return String(result);
}

String strPad(String value, uint8_t len) {
  char format[5];
  sprintf(format, "%%%ds", len);
  char result[len + 1];
  sprintf(result, format, value.c_str());
  return String(result);
}

double roundOff(double n) {
  double d = n * 10.0;
  int i = d + 0.5;
  d = (float)i / 10.0;
  return d;
}

String convertSize(unsigned long size_kb) {
  static const char *SIZES[] = { "KB", "MB", "GB", "TB" };
  int div = 0;
  unsigned long rem = 0;

  while (size_kb >= 1024 && div < (sizeof SIZES / sizeof * SIZES)) {
    rem = (size_kb % 1024);
    div++;
    size_kb /= 1024;
  }

  double size_d = (float)size_kb + (float)rem / 1024.0;
  String result = String(size_d, 1) + SIZES[div];
  return result;
}

#endif
