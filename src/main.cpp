/*
  PWR/SWR-Meter by Florian Thienel (DL3NEY)

  This software is based on a hardware design using two AD8307 and a tandem coupler,
  inspired by DL6GL and DG1KPN: http://dl6gl.de/digitales-swr-powermeter-mit-pep-anzeige

  This software is published under the MIT License: https://www.tldrlegal.com/l/mit
  (c) Florian Thienel

  Main Program
*/
#include <Arduino.h>
#include "ui.h"

const bool calibrate = false;

const int fwd37dbm = 838;
const int fwd50dbm = 969;
const int rev37dbm = 829;
const int rev50dbm = 962;
const float maxPwr = 100.0;

const float fwdFactor = float(fwd50dbm - fwd37dbm) / 13.0;
const float revFactor = float(rev50dbm - rev37dbm) / 13.0;

const int updateRate = calibrate ? 250 : 25;
int updateIndex = 0;

const int windowSize = 100;
int windowIndex = 0;
float fwdPwrValues[windowSize];
float fwdPwrSum = 0.0;
float fwdAvgPwr = 0.0;
float revPwrValues[windowSize];
float revPwrSum = 0.0;
float revAvgPwr = 0.0;

float fwdPeak = 0.0;
const int peakMaxHoldCount = 1500;
int peakHoldCount = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  analogReference(EXTERNAL);
  
  initUi();

  for (int i = 0; i < windowSize; i += 1) {
    fwdPwrValues[i] = 0.0;
  }
}

float reading2dbm(int reading, int ref37dbm, float factor) {
  return 37.0 + (float(reading - ref37dbm) / factor);
}

float dbm2watt(float dbm) {
  return pow(10.0, (dbm-30.0)/10.0);
}

float calcSwr(float fwdPwr, float revPwr) {
  if (fwdPwr <= 0.01) return 0.0;

  float s = sqrt(revPwr / fwdPwr);
  if (s >= 1.0) return 0.0;

  return (1.0 + s) / (1.0 - s);
}

void calcAvgPwr(float fwdValue, float revValue) {
  windowIndex = (windowIndex + 1) % windowSize;
  fwdPwrSum -= fwdPwrValues[windowIndex];
  revPwrSum -= revPwrValues[windowIndex];
  fwdPwrValues[windowIndex] = fwdValue;
  revPwrValues[windowIndex] = revValue;
  fwdPwrSum += fwdValue;
  revPwrSum += revValue;
  fwdAvgPwr = fwdPwrSum / float(windowSize);
  revAvgPwr = revPwrSum / float(windowSize);
}

void setFwdPeak(float value) {
  peakHoldCount += 1;
  if ((peakHoldCount >= peakMaxHoldCount) || (value > fwdPeak)) {
    peakHoldCount = 0;
    fwdPeak = value;
  }
}

void updateUi(float avgPwr, float peakPwr, float swr) {
  printFloat(0, 0, 5, avgPwr);
  printFloat(6, 0, 5, peakPwr);
  printFloat(13, 0, 3, swr);
  showBargraph(0, 1, 10, maxPwr, avgPwr);
  showBargraph(13, 1, 3, 3.0, swr - 1.0);
}

void updateCalibrateUi(int fwdReading, int revReading) {
  printText(0, 0, "Calibrate");
  printText(0, 1, "F:");
  printInt(2, 1, 4, fwdReading);
  printText(7, 1, "R:");
  printInt(9, 1, 4, revReading);
}

void loop() {
  int fwdReading = analogRead(A0);
  int revReading = analogRead(A1);

  float fwdDbm = reading2dbm(fwdReading, fwd37dbm, fwdFactor);
  float fwdPwr = dbm2watt(fwdDbm);
  float revDbm = reading2dbm(revReading, rev37dbm, revFactor);
  float revPwr = dbm2watt(revDbm);

  calcAvgPwr(fwdPwr, revPwr);
  setFwdPeak(fwdPwr);
  float swr = calcSwr(fwdAvgPwr, revAvgPwr);

  if (updateIndex == 0) {
    if (calibrate) {
      updateCalibrateUi(fwdReading, revReading);
    } else {
      updateUi(fwdAvgPwr, fwdPeak, swr);
    }
  }
  updateIndex = (updateIndex + 1) % updateRate;

  delay(1);
}

