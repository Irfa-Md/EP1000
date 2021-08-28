#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 RTC;

#define PIN 2
#define BTN 3

uint8_t NUM_DIGIT = 4;
const uint8_t LEDS_PER_DIGIT = 7;
uint16_t NUM_LEDS = 28;

uint8_t time_h = 0;
uint8_t time_m = 0;
int num = 0;
int val = 0;

float ambiant_light = 1.0;

bool mask_digit[10][7] = {
  { 1, 1, 1, 1, 1, 1, 0 },  // Number 0
  { 0, 0, 0, 0, 1, 1, 0 },  // Number 1
  { 1, 1, 1, 0, 0, 1, 1 },  // Number 2
  { 1, 0, 0, 1, 1, 1, 1 },  // Number 3
  { 1, 0, 1, 0, 1, 1, 0 },  // Number 4
  { 1, 1, 0, 1, 1, 0, 1 },  // Number 5
  { 1, 1, 1, 1, 1, 0, 1 },  // Number 6
  { 0, 0, 0, 0, 1, 1, 1 },  // Number 7
  { 1, 1, 1, 1, 1, 1, 1 },  // Number 8
  { 1, 1, 0, 1, 1, 1, 1 },  // Number 9
};
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
byte colors[3][3] = { { 0xff, 0, 0 },
                      { 0xff, 0xff, 0xff },
                      { 0, 0, 0xff } };

void setup() {
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  RTC.adjust(DateTime(0, 0, 0, 2, 5, 1));

  strip.begin();
  strip.show();  // Initialize all pixels to 'off'
  pinMode(BTN, INPUT);
}

void loop() {
  DateTime now = RTC.now();
  time_h = now.hour();
  time_m = now.minute();
  int digit_4 = time_h / 10;  //10 states, 0-9
  int digit_3 = time_h % 10;  //10 states, 0-9
  int digit_2 = time_m / 10;  //10 states, 0-9
  int digit_1 = time_m % 10;  //10 states, 0-9
  Serial.println(digit_4);
  Serial.println(digit_3);
  Serial.println(digit_2);
  Serial.println(digit_1);
  val = digitalRead(BTN);
  //  rainbowCycle(10, digit_4, digit_3, digit_2, digit_1, ambiant_light);

  byte *c;
  bool map_num[NUM_LEDS] = {};
  for (int i = 0; i < LEDS_PER_DIGIT; i++) {
    map_num[i] = mask_digit[digit_4][i];
  }
  for (int i = LEDS_PER_DIGIT; i < 2 * LEDS_PER_DIGIT; i++) {
    map_num[i] = mask_digit[digit_3][i - LEDS_PER_DIGIT];
  }
  for (int i = 2 * LEDS_PER_DIGIT; i < 3 * LEDS_PER_DIGIT; i++) {
    map_num[i] = mask_digit[digit_2][i + 2 - ((2 * LEDS_PER_DIGIT) + 2)];
  }
  for (int i = 3 * LEDS_PER_DIGIT; i < 4 * LEDS_PER_DIGIT; i++) {
    map_num[i] = mask_digit[digit_1][i - ((3 * LEDS_PER_DIGIT) + 2)];
  }
  for (uint16_t j = 256; j > 0; j--) {  // 5 cycles of all colors on wheel
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
      //      c = Wheel(((i * 256 / (NUM_LEDS/4)) + j) & 255); //each digit is rainbow
      c = Wheel(((i * 256 / (NUM_LEDS * 8)) + j) & 255);  //slow transition
      if (map_num[i] == 1 && val == HIGH) {
        uint8_t R = *c * ambiant_light;
        uint8_t G = *(c + 1) * ambiant_light;
        uint8_t B = *(c + 2) * ambiant_light;
        setPixel(i, R, G, B);
      } else
        setPixel(i, 0, 0, 0);
    }
    showStrip();
    delay(20);
  }
}


void rainbowCycle(uint8_t SpeedDelay, int d4, int d3, int d2, int d1, float ambiant_light) {
  byte *c;
  uint16_t i, j;
  bool map_num[NUM_LEDS] = {};
  for (int i = 0; i < LEDS_PER_DIGIT; i++) {
    map_num[i] = mask_digit[d4][i];
  }
  for (int i = LEDS_PER_DIGIT; i < 2 * LEDS_PER_DIGIT; i++) {
    map_num[i] = mask_digit[d3][i - LEDS_PER_DIGIT];
  }
  for (int i = 2 * LEDS_PER_DIGIT; i < 3 * LEDS_PER_DIGIT; i++) {
    map_num[i] = mask_digit[d2][i - 2 * LEDS_PER_DIGIT];
  }
  for (int i = 3 * LEDS_PER_DIGIT; i < 4 * LEDS_PER_DIGIT; i++) {
    map_num[i] = mask_digit[d1][i - 3 * LEDS_PER_DIGIT];
  }

  for (j = 0; j < 256; j++) {  // 5 cycles of all colors on wheel
    for (i = 0; i < NUM_LEDS; i++) {
      c = Wheel(((i * 256 / (NUM_LEDS)) + j) & 255);
      if (map_num[i] == 1) {
        uint8_t R = *c * ambiant_light;
        uint8_t G = *(c + 1) * ambiant_light;
        uint8_t B = *(c + 2) * ambiant_light;
        setPixel(i, R, G, B);
      } else
        setPixel(i, 0, 0, 0);
    }
    showStrip();
    delay(1000);
  }
}

// *** REPLACE TO HERE ***

void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

void blink() {

  strip.setPixelColor(14, 255, 255, 255);
  strip.setPixelColor(15, 255, 255, 255);
  strip.show();  // Send the updated pixel colors to the hardware.

  delay(500);
  strip.clear();
  strip.show();
  delay(500);
}
void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
    delay(1);
  }
  showStrip();
}

byte *Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}
