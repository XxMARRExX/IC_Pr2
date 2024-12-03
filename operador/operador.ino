/* 
 * srf02_example.ino
 * Example showing how to use the Devantech SRF02 ultrasonic sensor
 * in I2C mode. More info about this LCD display in
 *   http://www.robot-electronics.co.uk/htm/srf02techI2C.htm
 *
 * author: Antonio C. Domínguez Brito <adominguez@iusiani.ulpgc.es>
 */
 
/* 
 * Arduino MKR WAN 1310 I2C pins
 *   Pin 11 -> SDA (I2C data line)
 *   Pin 12 -> SCL (I2C clock line)
 */
 
#include <Wire.h> // Arduino's I2C library

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <sam.h>

//#define TCC0_BASE           ((void *)0x42002000UL)

// OLED display width and height in pixels
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// SRF02's I2C address and command related definitions 
#define SRF02_I2C_ADDRESS_1 byte((0xE0)>>1)
#define SRFO2_I2C_ADDRESS_2 byte((0xE2)>>1) // DIRECCION POR DECIDIR ----------------
#define SRF02_I2C_INIT_DELAY 100 // in milliseconds
#define SRF02_RANGING_DELAY 70 // milliseconds
#define tabulador "    "

// LCD05's command related definitions
#define COMMAND_REGISTER byte(0x00)
#define SOFTWARE_REVISION byte(0x00)
#define RANGE_HIGH_BYTE byte(2)
#define RANGE_LOW_BYTE byte(3)
#define AUTOTUNE_MINIMUM_HIGH_BYTE byte(4)
#define AUTOTUNE_MINIMUM_LOW_BYTE byte(5)

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

struct Sensor {
  String name;
  byte address;
  byte unit;
  int min_delay;
  int period_delay;
  bool shotting;
  String last_shot;
};

Sensor sensor1;
Sensor sensor2;

volatile int flag1 = 0;
volatile int flag2 = 0;

int contador = 0;

inline void write_command(byte address, byte command)
{ 
  Wire.beginTransmission(address);
  Wire.write(COMMAND_REGISTER); 
  Wire.write(command); 
  Wire.endTransmission();
}

byte read_register(byte address,byte the_register)
{
  Wire.beginTransmission(address);
  Wire.write(the_register);
  Wire.endTransmission();
  
  // getting sure the SRF02 is not busy
  Wire.requestFrom(address,byte(1));
  while(!Wire.available()) { /* do nothing */ }
  return Wire.read();
} 
 
// the setup routine runs once when you press reset:
void setup() 
{
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  Serial.println("initializing Wire interface ...");
  Wire.begin();
  delay(SRF02_I2C_INIT_DELAY);  

  display.display();
  delay(2000); // Pause for 2 seconds
  
  /*
  byte software_revision=read_register(SRF02_I2C_ADDRESS,SOFTWARE_REVISION);
  Serial.print("SFR02 ultrasonic range finder in address 0x");
  Serial.print(SRF02_I2C_ADDRESS,HEX); Serial.print("(0x");
  Serial.print(software_revision,HEX); Serial.println(")");*/


  init_sensors();
  
  init_TCC0();
  init_TC4_TC5();

  probarCodigo();

  drawOLED();
}

void probarCodigo(){
  shot(sensor1);
  set_sensor_on_period(sensor2, 1000);
  set_sensor_on_period(sensor1, 2000);
  unit_cm(sensor1);
}

// the loop routine runs over and over again forever:
void loop() 
{
  if (flag1){
    flag1 = 0;
    shot(sensor1);
  }

  if (flag2){
    flag2 = 0;
    shot(sensor2);
  }
}

//ZONA DE TIMERS ---------------------------------------------------------------

/* Iniciar el temporizador tcc0 encargado del sensor 1 */
void init_TCC0() {
    // Enable power management clock for TCC0
    PM->APBCMASK.reg |= PM_APBCMASK_TCC0;

    // Configure GCLK for TCC0
    GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN_GCLK0 |
                        GCLK_CLKCTRL_ID(GCM_TCC0_TCC1));
    while (GCLK->STATUS.bit.SYNCBUSY);

    // Disable TCC0
    TCC0->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
    while (TCC0->SYNCBUSY.bit.ENABLE);

    // Reset TCC0
    TCC0->CTRLA.reg = TCC_CTRLA_SWRST;
    while (TCC0->SYNCBUSY.bit.SWRST);

    // Configure TCC0 with maximum period
    TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1024 |
                      TCC_CTRLA_PRESCSYNC_PRESC;
    while (TCC0->SYNCBUSY.bit.ENABLE);

    // Set maximum period
    TCC0->PER.reg = 0xFFFFFFFF;
    while (TCC0->SYNCBUSY.bit.PER);

    // Enable TCC0
    TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE;
    while (TCC0->SYNCBUSY.bit.ENABLE);
}

/*Desactivar interrupción TCC0 para sensor 1*/
void disable_tcc0_interrupt() {
    NVIC_DisableIRQ(TCC0_IRQn);
    TCC0->INTENCLR.reg = TCC_INTENCLR_OVF;  // Disable overflow interrupt
}

/*Activar interrupción TCC0 para sensor 1*/
void enable_tcc0_interrupt() {
    TCC0->INTENSET.reg = TCC_INTENSET_OVF;   // Enable overflow interrupt
    NVIC_EnableIRQ(TCC0_IRQn);
}

/*Establecer periodo de interrupcion de TCC0 para sensor 1*/
void set_tcc0_period(int ms) {
    uint32_t period = (uint32_t)(ms * (48000000 / 1024) / 1000);
    TCC0->PER.reg = period;
    while (TCC0->SYNCBUSY.bit.PER);
}

/* Iniciar el temporizador tc4 y tc5 encargado del sensor 2 */
void init_TC4_TC5() {
    // Enable power management clock for TC4 and TC5
    PM->APBCMASK.reg |= PM_APBCMASK_TC4 | PM_APBCMASK_TC5;

    // Configure GCLK for TC4 and TC5
    GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN_GCLK0 |
                        GCLK_CLKCTRL_ID(GCM_TC4_TC5));
    while (GCLK->STATUS.bit.SYNCBUSY);

    // Disable TC4 and TC5
    TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
    TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
    while (TC5->COUNT16.STATUS.bit.SYNCBUSY);

    // Reset TC4 and TC5
    TC4->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
    while (TC5->COUNT16.STATUS.bit.SYNCBUSY);

    // Configure TC4 and TC5 for 32-bit counter mode
    TC4->COUNT32.CTRLA.reg = TC_CTRLA_MODE_COUNT32 |    // 32-bit mode
                             TC_CTRLA_PRESCALER_DIV1024 | // Same prescaler as TCC0
                             TC_CTRLA_WAVEGEN_NFRQ;      // Normal Frequency mode
    
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY);

    // Set maximum period
    TC4->COUNT32.CC[0].reg = 0xFFFFFFFF;
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY);

    // Enable TC4 (TC5 is part of TC4 in 32-bit mode)
    TC4->COUNT32.CTRLA.reg |= TC_CTRLA_ENABLE;
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY);
}

/*Desactivar interrupción TC4 y TC5 para sensor 2*/
void disable_tc4_tc5_interrupt() {
    NVIC_DisableIRQ(TC4_IRQn);
    TC4->COUNT32.INTENCLR.reg = TC_INTENCLR_MC0;  // Disable match compare interrupt
}

/*Activar interrupción TC4 y TC5 para sensor 2*/
void enable_tc4_tc5_interrupt() {
    TC4->COUNT32.INTENSET.reg = TC_INTENSET_MC0;   // Enable match compare interrupt
    NVIC_EnableIRQ(TC4_IRQn);
}

/*Establecer periodo de interrupcion de TC4 y TC5 para sensor 2*/
void set_tc4_tc5_period(int ms) {
    uint32_t period = (uint32_t)(ms * (48000000 / 1024) / 1000);
    TC4->COUNT32.CC[0].reg = period;
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY);
}

//ZONA DE OLED -----------------------------------------------------------------

/*Actualizar pantalla OLED*/
void drawOLED(){
  Serial.print("drawOLED... ");
  display.clearDisplay();

  drawSensorInformationSensor1();
  drawSensorInformationSensor2();
  //drawCont();
  Serial.println("done");
}

void drawCont(){
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("Contador: ")); display.println(contador);

  display.display();

  contador++;
}

/*Dibujar información del sensor 1 en la pantalla OLED*/
void drawSensorInformationSensor1(){

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

/*Dibujar información del sensor 2 en la pantalla OLED*/
void drawSensorInformationSensor2(){

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

//ZONA DE SENSORES -------------------------------------------------------------

/*Inicializar sensores*/
void init_sensors(){
  sensor1.name = "Sensor 1";
  sensor1.address = SRF02_I2C_ADDRESS_1;
  sensor1.unit = REAL_RANGING_MODE_INCHES;
  sensor1.min_delay = SRF02_RANGING_DELAY;
  sensor1.period_delay = 1000; // por poner algo
  sensor1.shotting = false;
  sensor1.last_shot = " ";

  sensor2.name = "Sensor 2";
  sensor2.address = SRFO2_I2C_ADDRESS_2;
  sensor2.unit = REAL_RANGING_MODE_INCHES;
  sensor2.min_delay = SRF02_RANGING_DELAY;
  sensor2.period_delay = 1000; // por poner algo
  sensor2.shotting = false;
  sensor2.last_shot = " ";
}

/*Cambiar unidad de medida a cm*/
void unit_cm(Sensor& sensor){
  sensor.unit = REAL_RANGING_MODE_CMS;
}

/*Cambiar unidad de medida a pulgadas*/
void unit_inches(Sensor& sensor){
  sensor.unit = REAL_RANGING_MODE_INCHES;
}

/*Cambiar unidad de medida a microsegundos*/
void unit_ms(Sensor& sensor){
  sensor.unit = REAL_RANGING_MODE_USECS;
}

/*
Realizar una medición con el sensor

@return int: distancia en la unidad de medida del sensor
*/
int shot(Sensor& sensor){
  write_command(sensor.address,sensor.unit);
  delay(sensor.min_delay);

  /*
  while (read_register(sensor.address,COMMAND_REGISTER) == 0xFF){
    delay(1);
  }*/

  //Serial.print("READ REGISTER...");
  byte high_byte_range=read_register(sensor.address,RANGE_HIGH_BYTE);
  //Serial.print(" first part read...");
  byte low_byte_range=read_register(sensor.address,RANGE_LOW_BYTE);
  //Serial.println(" done");
  

  int result = (int)((high_byte_range << 8) | low_byte_range);
  String unit_str;

  switch(sensor.unit) {
      case REAL_RANGING_MODE_INCHES:
          unit_str = " inches";
          break;
      case REAL_RANGING_MODE_CMS:
          unit_str = " cm";
          break;
      case REAL_RANGING_MODE_USECS:
          unit_str = " us";
          break;
      default:
          unit_str = "";
  }

  sensor.last_shot = String(result) + unit_str;

  drawOLED();

  return result;
}

/*
Realizar disparos periodicos (periodo debe ser mayor o igual que el delay mínimo del sensor)
@return bool: true si se ha cambiado correctamente, false en caso contrario
*/
bool set_sensor_on_period(Sensor& sensor, int period){
  if (period < sensor.min_delay){
    return false;
  }

  sensor.shotting = true;
  sensor.period_delay = period;

  if (sensor.address == SRF02_I2C_ADDRESS_1){
    set_tcc0_period(period);
    enable_tcc0_interrupt();
  } else {
    set_tc4_tc5_period(period);
    enable_tc4_tc5_interrupt();
  }

  drawOLED();

  return true;
}

/*Detener disparos periodicos*/
void set_sensor_off(Sensor& sensor){
  sensor.shotting = false;

  if (sensor.address == SRF02_I2C_ADDRESS_1){
    disable_tcc0_interrupt();
  } else {
    disable_tc4_tc5_interrupt();
  }

  drawOLED();
}

/*
Cambia el delay mínimo de un sensor (mínimo posible 70 ms)

@return bool: true si se ha cambiado correctamente, false en caso contrario
*/
bool set_sensor_delay(Sensor& sensor, int delay) {
    if (delay < SRF02_RANGING_DELAY) {
        return false;
    }
    sensor.min_delay = delay;
    return true;
}

/*Devolver el status de un sensor*/
void status(Sensor& sensor){ // falta modificar para añadir el uso del bus CAN
  Serial.print("Sensor: "); Serial.print(sensor.address);
  Serial.print(" unit: "); Serial.print(sensor.unit);
  Serial.print(" shotting: "); Serial.print(sensor.shotting);
  Serial.print(" period: "); Serial.print(sensor.period_delay);
  Serial.println();
}

//ZONA DE INTERRUPCIONES -------------------------------------------------------

/*Interrupción del timer TCC0 que controla el sensor 1*/
void TCC0_Handler() {
    if (TCC0->INTFLAG.bit.OVF) {
        TCC0->INTFLAG.reg = TCC_INTFLAG_OVF;
        flag1 = 1;
    }
}

/*Interrupción del timer TC4 que controla el sensor 2*/
void TC4_Handler() {
    if (TC4->COUNT32.INTFLAG.bit.MC0) {
        TC4->COUNT32.INTFLAG.bit.MC0 = 1;    // Clear the interrupt flag
        TC4->COUNT32.COUNT.reg = 0;          // Reset counter to zero
        while (TC4->COUNT32.STATUS.bit.SYNCBUSY); // Wait for sync
        flag2 = 1;
    }
}
