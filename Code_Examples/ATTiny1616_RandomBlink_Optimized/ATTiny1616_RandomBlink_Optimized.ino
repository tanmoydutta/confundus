#include <avr/sleep.h>
#include <avr/interrupt.h>

// Row pins
const uint8_t rowPins[8] = {
  PIN_PC0, PIN_PC1, PIN_PC2, PIN_PC3,
  PIN_PB0, PIN_PB1, PIN_PB2, PIN_PB3
};

// Column pin
const uint8_t colPin = PIN_PA1;

volatile bool wakeFlag = false;

void setup() {
  allHiZ();

  ADC0.CTRLA &= ~ADC_ENABLE_bm;
  AC0.CTRLA  &= ~AC_ENABLE_bm;

  randomSeed(analogRead(0));

  setupRTC();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void loop() {

  if (!wakeFlag) {
    sleep_mode();
  }

  wakeFlag = false;

  // Let boost stabilize
  delay(1);

  // ✨ Burst of sparkles
  for (uint8_t i = 0; i < 4; i++) {

    uint8_t r = random(0, 8);

    lightOne(r);
    delay(5);     // ON time

    allHiZ();
    delay(20);    // short gap between sparkles
  }
}

// Turn on one LED
void lightOne(uint8_t row) {
  allHiZ();

  pinMode(colPin, OUTPUT);
  digitalWrite(colPin, HIGH);

  pinMode(rowPins[row], OUTPUT);
  digitalWrite(rowPins[row], LOW);
}

// Hi-Z all pins
void allHiZ() {
  pinMode(colPin, INPUT);

  for (uint8_t i = 0; i < 8; i++) {
    pinMode(rowPins[i], INPUT);
  }
}

// RTC setup (~2 sec)
void setupRTC() {
  while (RTC.STATUS > 0);

  RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
  RTC.PITINTCTRL = RTC_PI_bm;

  RTC.PITCTRLA = RTC_PERIOD_CYC2048_gc | RTC_PITEN_bm;

  sei();
}

ISR(RTC_PIT_vect) {
  RTC.PITINTFLAGS = RTC_PI_bm;
  wakeFlag = true;
}