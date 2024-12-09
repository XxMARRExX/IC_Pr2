#include "oled_display.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/**
 * @brief Actualiza la pantalla OLED con información de los sensores.
 */
void drawOLED(){
  Serial.print("drawOLED... ");
  display.clearDisplay();

  drawSensorInformationSensor1();
  drawSensorInformationSensor2();
  //drawCont();
  Serial.println("done");
}

/**
 * @brief Dibuja información del sensor 1 en la pantalla OLED.
 */
void drawSensorInformationSensor1() {
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("Sensor 1:"));
  display.setTextColor(SSD1306_WHITE);
  if (sensor1.shotting){
    display.print(F(" on ")); display.print(sensor1.period_delay); display.println(F(" ms"));
  } else {
    display.println(F(" off"));
  }

  display.println();

  display.print(F("Last shot: ")); display.println(sensor1.last_shot);

  display.display();
}

/**
 * @brief Dibuja información del sensor 2 en la pantalla OLED.
 */
void drawSensorInformationSensor2() {
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);        // Draw white text
  int middleY = (64 - 8) / 2;
  display.setCursor(0,middleY+4);             // Start at top-left corner
  display.print(F("Sensor 2:"));
  display.setTextColor(SSD1306_WHITE);
  if (sensor2.shotting){
    display.print(F(" on ")); display.print(sensor2.period_delay); display.println(F(" ms"));
  } else {
    display.println(F(" off"));
  }

  display.println();

  display.print(F("Last shot: ")); display.println(sensor2.last_shot);

  display.display();
}
