#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "sensors.h"

// Display constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3D

// Global OLED display variable
extern Adafruit_SSD1306 display;

// Function declarations
void drawOLED();
void drawSensorInformationSensor1();
void drawSensorInformationSensor2();
void turnOnOLED();
void turnOffOLED();

#endif // OLED_DISPLAY_H