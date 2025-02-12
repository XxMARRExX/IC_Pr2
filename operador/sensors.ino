#include "sensors.h"

Sensor sensor1;
Sensor sensor2;

Sensor oled = {"OLED", 0x3D, 0, 0, 0, false, ""};

/**
 * @brief Inicializa los sensores configurando sus parámetros iniciales.
 */
void init_sensors() {
  //name, address, unit, min_delay, period_delay, shotting, last_shot
  sensor1 = {"Sensor 1", SRF02_I2C_ADDRESS_1, REAL_RANGING_MODE_CMS, SRF02_RANGING_DELAY, 1000, false, ""};
  sensor2 = {"Sensor 2", SRFO2_I2C_ADDRESS_2, REAL_RANGING_MODE_CMS, SRF02_RANGING_DELAY, 1000, false, ""};
}


/**
 * @brief Cambia la unidad de medida de un sensor a centímetros.
 * 
 * @param sensor Referencia al sensor a configurar.
 */
void unit_cm(Sensor& sensor) {
  sensor.unit = REAL_RANGING_MODE_CMS;
}


/**
 * @brief Cambia la unidad de medida de un sensor a pulgadas.
 * 
 * @param sensor Referencia al sensor a configurar.
 */
void unit_inches(Sensor& sensor) {
  sensor.unit = REAL_RANGING_MODE_INCHES;
}


/**
 * @brief Cambia la unidad de medida de un sensor a microsegundos.
 * 
 * @param sensor Referencia al sensor a configurar.
 */
void unit_ms(Sensor& sensor) {
  sensor.unit = REAL_RANGING_MODE_USECS;
}


/**
 * @brief Realiza una medición con el sensor y devuelve la distancia medida.
 * 
 * @param sensor Referencia al sensor a usar.
 * @return int Distancia medida en la unidad de medida configurada.
 */
int shot(Sensor& sensor) {
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


/**
 * @brief Configura un sensor para realizar mediciones periódicas.
 * 
 * @param sensor Referencia al sensor a configurar.
 * @param period Periodo en milisegundos entre cada medición.
 * @return bool true si la configuración es exitosa, false si el periodo es inválido.
 */
bool set_sensor_on_period(Sensor& sensor, int period) {
  if (period < sensor.min_delay){
    return false;
  }

  sensor.shotting = true;
  sensor.period_delay = period;

  if (sensor.address == SRF02_I2C_ADDRESS_1){
    start_timer_1(period);
  } else {
    start_timer_2(period);
  }

  drawOLED();

  return true;
}


/**
 * @brief Detiene las mediciones periódicas de un sensor.
 * 
 * @param sensor Referencia al sensor a configurar.
 */
void set_sensor_off(Sensor& sensor) {
  sensor.shotting = false;

  if (sensor.address == SRF02_I2C_ADDRESS_1){
    stop_timer_1();
  } else {
    stop_timer_2();
  }

  drawOLED();
}


/**
 * @brief Cambia el delay mínimo de un sensor.
 * 
 * @param sensor Referencia al sensor a configurar.
 * @param delay Nuevo valor de delay mínimo.
 * @return bool true si la configuración es exitosa, false si el valor es inválido.
 */
bool set_sensor_delay(Sensor& sensor, int delay) {
  if (delay < SRF02_RANGING_DELAY) {
        return false;
    }
    sensor.min_delay = delay;
    return true;
}


/**
 * @brief Envía un comando al sensor a través del bus I2C.
 * 
 * @param address Dirección I2C del sensor.
 * @param command Comando a enviar.
 */
void write_command(byte address, byte command) {
  Wire.beginTransmission(address);
  Wire.write(COMMAND_REGISTER); 
  Wire.write(command); 
  Wire.endTransmission();
}


/**
 * @brief Lee un registro del sensor a través del bus I2C.
 * 
 * @param address Dirección I2C del sensor.
 * @param reg Registro a leer.
 * @return byte Valor leído del registro.
 */
byte read_register(byte address, byte the_register) {
  Wire.beginTransmission(address);
  Wire.write(the_register);
  Wire.endTransmission();
  
  // getting sure the SRF02 is not busy
  Wire.requestFrom(address,byte(1));
  while(!Wire.available()) { /* do nothing */ }
  return Wire.read();
}