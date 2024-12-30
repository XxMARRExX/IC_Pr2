#ifndef SENSORS_H
#define SENSORS_H

#include <Wire.h>
#include "oled_display.h"

// Constants definitions
#define SRF02_I2C_ADDRESS_1 byte((0xE0)>>1)
#define SRFO2_I2C_ADDRESS_2 byte((0xE2)>>1)
#define SRF02_I2C_INIT_DELAY 100
#define SRF02_RANGING_DELAY 70
#define tabulador "    "

// SRF02's command codes
#define REAL_RANGING_MODE_INCHES    byte(80)
#define REAL_RANGING_MODE_CMS       byte(81)
#define REAL_RANGING_MODE_USECS     byte(82)
#define FAKE_RANGING_MODE_INCHES    byte(86)
#define FAKE_RANGING_MODE_CMS       byte(87)
#define FAKE_RANGING_MODE_USECS     byte(88)
#define TRANSMIT_8CYCLE_40KHZ_BURST byte(92)
#define FORCE_AUTOTUNE_RESTART      byte(96)
#define ADDRESS_CHANGE_1ST_SEQUENCE byte(160)
#define ADDRESS_CHANGE_3RD_SEQUENCE byte(165)
#define ADDRESS_CHANGE_2ND_SEQUENCE byte(170)

#define COMMAND_REGISTER byte(0x00)
#define SOFTWARE_REVISION byte(0x00)
#define RANGE_HIGH_BYTE byte(2)
#define RANGE_LOW_BYTE byte(3)
#define AUTOTUNE_MINIMUM_HIGH_BYTE byte(4)
#define AUTOTUNE_MINIMUM_LOW_BYTE byte(5)

// Sensor structure definition
struct Sensor {
    String name;
    byte address;
    byte unit;
    int min_delay;
    int period_delay;
    bool shotting;
    String last_shot;
};

// Global sensor variables
extern Sensor sensor1;
extern Sensor sensor2;
extern Sensor oled;

// Function declarations
void init_sensors();
void unit_cm(Sensor& sensor);
void unit_inches(Sensor& sensor);
void unit_ms(Sensor& sensor);
int shot(Sensor& sensor);
bool set_sensor_on_period(Sensor& sensor, int period);
void set_sensor_off(Sensor& sensor);
bool set_sensor_delay(Sensor& sensor, int delay);
void write_command(byte address, byte command);
byte read_register(byte address, byte the_register);

// External function declarations (referenced but not defined in sensors.ino)
void drawOLED();
void set_tcc0_period(int period);
void enable_tcc0_interrupt();
void disable_tcc0_interrupt();
void set_tc4_tc5_period(int period);
void enable_tc4_tc5_interrupt();
void disable_tc4_tc5_interrupt();

#endif // SENSORS_H