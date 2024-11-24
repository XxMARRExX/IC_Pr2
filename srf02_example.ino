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
 
#define SRF02_I2C_ADDRESS_1 byte((0xE0)>>1)
#define SRFO2_I2C_ADDRESS_2 byte((0xE2)>>1) // DIRECCION POR DECIDIR ----------------
#define SRF02_I2C_INIT_DELAY 100 // in milliseconds
#define SRF02_RANGING_DELAY 70 // milliseconds

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
  byte address;
  byte unit;
  byte dir_i2c;
  int min_delay;
  int period_delay;
  bool shotting;
};

Sensor sensor1;
Sensor sensor2;

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
  
  Serial.println("initializing Wire interface ...");
  Wire.begin();
  delay(SRF02_I2C_INIT_DELAY);  
  
  /*
  byte software_revision=read_register(SRF02_I2C_ADDRESS,SOFTWARE_REVISION);
  Serial.print("SFR02 ultrasonic range finder in address 0x");
  Serial.print(SRF02_I2C_ADDRESS,HEX); Serial.print("(0x");
  Serial.print(software_revision,HEX); Serial.println(")");*/

  init_sensors();
  unit_cm(sensor1);
}

// the loop routine runs over and over again forever:
void loop() 
{
  /*
  Serial.print("ranging ...");
  write_command(SRF02_I2C_ADDRESS,REAL_RANGING_MODE_CMS);
  delay(SRF02_RANGING_DELAY);
  
  byte high_byte_range=read_register(SRF02_I2C_ADDRESS,RANGE_HIGH_BYTE);
  byte low_byte_range=read_register(SRF02_I2C_ADDRESS,RANGE_LOW_BYTE);
  byte high_min=read_register(SRF02_I2C_ADDRESS,AUTOTUNE_MINIMUM_HIGH_BYTE);
  byte low_min=read_register(SRF02_I2C_ADDRESS,AUTOTUNE_MINIMUM_LOW_BYTE);
  
  Serial.print(int((high_byte_range<<8) | low_byte_range)); Serial.print(" cms. (min=");
  Serial.print(int((high_min<<8) | low_min)); Serial.println(" cms.)");
  
  delay(1000);
  */
  Serial.print("Distancia sensor 1: "); Serial.print(shot(sensor1)); Serial.println(" cms.");

}

//cosecha propia ------------------------------
void init_sensors(){
  sensor1.address = SRF02_I2C_ADDRESS_1;
  sensor1.unit = REAL_RANGING_MODE_INCHES;
  sensor1.dir_i2c = SRF02_I2C_ADDRESS_1;
  sensor1.min_delay = SRF02_RANGING_DELAY;
  sensor1.period_delay = 1000; // por poner algo
  sensor1.shotting = false;

  sensor2.address = SRFO2_I2C_ADDRESS_2;
  sensor2.unit = REAL_RANGING_MODE_INCHES;
  sensor2.dir_i2c = SRFO2_I2C_ADDRESS_2;
  sensor2.min_delay = SRF02_RANGING_DELAY;
  sensor2.period_delay = 1000; // por poner algo
  sensor2.shotting = false;
}

void unit_cm(Sensor& sensor){
  sensor.unit = REAL_RANGING_MODE_CMS;
}

void unit_inches(Sensor& sensor){
  sensor.unit = REAL_RANGING_MODE_INCHES;
}

void unit_ms(Sensor& sensor){
  sensor.unit = REAL_RANGING_MODE_USECS;
}

int shot(Sensor& sensor){
  write_command(sensor.address,sensor.unit);
  delay(sensor.min_delay);

  byte high_byte_range=read_register(sensor.address,RANGE_HIGH_BYTE);
  byte low_byte_range=read_register(sensor.address,RANGE_LOW_BYTE);

  return int((high_byte_range<<8) | low_byte_range);
}

void set_period_shotting(Sensor& sensor, int period){
  sensor.shotting = true;
  sensor.period_delay = period;
}

void stop_period_shotting(Sensor& sensor){
  sensor.shotting = false;
}

void status(Sensor& sensor){ // falta modificar para añadir el uso del bus CAN
  Serial.print("Sensor: "); Serial.print(sensor.dir_i2c);
  Serial.print(" unit: "); Serial.print(sensor.unit);
  Serial.print(" shotting: "); Serial.print(sensor.shotting);
  Serial.print(" period: "); Serial.print(sensor.period_delay);
  Serial.println();
}

//cosecha propia ------------------------------






