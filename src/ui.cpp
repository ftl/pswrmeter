/*
  PWR/SWR-Meter by Florian Thienel (DL3NEY)

  This software is published under the MIT License: https://www.tldrlegal.com/l/mit
  (c) Florian Thienel

  User Interface
*/
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "lcdbars.h"

LiquidCrystal lcd(12, 13, 8, 9, 10, 11);

void initUi() {
  lcd.createChar(0, bar0);
  lcd.createChar(1, bar1);
  lcd.createChar(2, bar2);
  lcd.createChar(3, bar3);
  lcd.createChar(4, bar4);
  lcd.createChar(5, bar5);
  lcd.begin(16, 2);
}

void showBargraph(int column, int row, int width, float max, float reading) {
  
  float percent = max(0.0, min(100.0, (reading / max) * 100.0));
  float perUnit = 100.0 / float(width);
  float perBar = perUnit / 5.0;

  lcd.setCursor(column, row);
  for (int i = 0; i < width; i+= 1) {
    float part = percent / perBar;
    if (part >= 5.0) {
      lcd.write(5);
    } else {
      lcd.write(int(part));
    }
    percent = max(0.0, percent - perUnit);
  }
}

int calcIntWidth(int integer) {
  int a = abs(integer);
  int base = 10;
  int w = 1;
  while ((a != a % base) && w < 6) {
      base *= 10;
      w += 1;
  }
  return w;
}

void printFloat(int column, int row, int width, float f) {
  float rounded = round(f * 10.0);
  int integer = int(rounded / 10.0);
  int decimal = int(abs(rounded)) % 10;
  int w = calcIntWidth(integer);
  if (integer < 0) w += 1;

  lcd.setCursor(column, row);
  for (int i = 0; i < width - 2 - w; i += 1) {
    lcd.print(" ");
  }
  lcd.print(integer);
  lcd.print(".");
  lcd.print(decimal);
}

void printInt(int column, int row, int width, int integer) {
    int w = calcIntWidth(integer);
    lcd.setCursor(column, row);
    for (int i = 0; i < width - w; i += 1) {
        lcd.print(" ");
    }
    lcd.print(integer);
}

void printText(int column, int row, const char *text) {
    lcd.setCursor(column, row);
    lcd.print(text);
}