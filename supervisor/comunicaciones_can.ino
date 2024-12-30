#define CAN_COMMAND_ID 0x200
#define CAN_COMMAND_ACK 0x201

/**
 * @brief Envía un comando "one-shot" al dispositivo especificado a través del bus CAN.
 * 
 * Este comando indica que el dispositivo debe realizar una medición única.
 * 
 * @param deviceAddrss Dirección del dispositivo (byte).
 */
void oneshot_to_can(byte deviceAddrss) {

  byte data[8] = {0x00, deviceAddrss};

  if (CAN.sendMsgBuf(CAN_COMMAND_ID, 0, 6, data) == CAN_OK) { 
    if (!waitForAck(deviceAddrss)) {
      Serial.println("Error: No se recibió ACK.");
    }
  } else {
    Serial.println("Error enviando mensaje.");
  }
}


/**
 * @brief Activa un dispositivo con un tiempo específico.
 * 
 * Este comando configura un dispositivo para que permanezca activo durante un periodo de tiempo determinado.
 * 
 * @param deviceAddrss Dirección del dispositivo (byte).
 * @param valor Tiempo en milisegundos (int).
 */
void us_on(byte deviceAddrss, int valor) {

  byte data[8] = {0x01, deviceAddrss};

  data[2] = valor & 0xFF; 
  data[3] = (valor >> 8) & 0xFF; 
  data[4] = (valor >> 16) & 0xFF;  
  data[5] = (valor >> 24) & 0xFF;

  /*
  Serial.println("Bytes enviados: ");
  Serial.println(data[0], BIN);
  Serial.println(data[1], BIN);
  Serial.println(data[2], BIN);
  Serial.println(data[3], BIN);
  Serial.println(data[4], BIN);
  Serial.println(data[5], BIN);
  */

  if (CAN.sendMsgBuf(CAN_COMMAND_ID, 0, 6, data) == CAN_OK) { 
    if (!waitForAck(deviceAddrss)) {
      Serial.println("Error: No se recibió ACK.");
    }
  } else {
    Serial.println("Error enviando mensaje.");
  }
}


/**
 * @brief Desactiva un dispositivo a través del bus CAN.
 * 
 * Este comando indica que el dispositivo debe detener su funcionamiento.
 * 
 * @param deviceAddrss Dirección del dispositivo (byte).
 */
void us_off(byte deviceAddrss) {

  byte data[8] = {0x02, deviceAddrss};

  if (CAN.sendMsgBuf(CAN_COMMAND_ID, 0, 6, data) == CAN_OK) { 
    if (!waitForAck(deviceAddrss)) {
      Serial.println("Error: No se recibió ACK.");
    }
  } else {
    Serial.println("Error enviando mensaje.");
  }
}


/**
 * @brief Configura la unidad de medida de un dispositivo.
 * 
 * Este comando establece si el dispositivo trabaja con pulgadas, centímetros o milisegundos.
 * 
 * @param unit Unidad de medida (`0x10` para pulgadas, `0x11` para centímetros, `0x12` para milisegundos).
 * @param deviceAddrss Dirección del dispositivo (byte).
 */
void us_unit(byte unit, byte deviceAddrss) {

  byte data[8] = {unit, deviceAddrss};

  if (CAN.sendMsgBuf(CAN_COMMAND_ID, 0, 6, data) == CAN_OK) { 
    if (!waitForAck(deviceAddrss)) {
      Serial.println("Error: No se recibió ACK.");
    }
  } else {
    Serial.println("Error enviando mensaje.");
  }
}


/**
 * @brief Configura un retraso o intervalo de medición para un dispositivo.
 * 
 * Este comando establece un tiempo en milisegundos para el retraso o intervalo entre mediciones.
 * 
 * @param deviceAddrss Dirección del dispositivo (byte).
 * @param valor Tiempo en milisegundos (int).
 */
void us_delay(byte deviceAddrss, int valor) {

  byte data[8] = {0x20, deviceAddrss};

  Serial.println("Bytes enviados");
  Serial.println(data[0], BIN);
  Serial.println(data[1], BIN);

  data[2] = valor & 0xFF; 
  data[3] = (valor >> 8) & 0xFF; 
  data[4] = (valor >> 16) & 0xFF;  
  data[5] = (valor >> 24) & 0xFF;       

  if (CAN.sendMsgBuf(CAN_COMMAND_ID, 0, 6, data) == CAN_OK) { 
    if (!waitForAck(deviceAddrss)) {
      Serial.println("Error: No se recibió ACK.");
    }
  } else {

    Serial.println("Error enviando mensaje.");
  }
}


void switch_oled(byte operation, byte deviceAddrss){
  byte data[8] = {operation, deviceAddrss};    

  if (CAN.sendMsgBuf(CAN_COMMAND_ID, 0, 6, data) == CAN_OK) { 
    if (!waitForAck(deviceAddrss)) {
      Serial.println("Error: No se recibió ACK.");
    }
  } else {
    Serial.println("Error enviando mensaje.");
  }
}


/**
 * @brief Espera un ACK (Acknowledgment) de un dispositivo en el bus CAN.
 * 
 * Este método espera hasta 1 segundo para recibir una confirmación del dispositivo indicado.
 * 
 * @param expectedDeviceAddress Dirección del dispositivo que se espera confirme (byte).
 * @return true Si se recibe un ACK válido del dispositivo esperado.
 * @return false Si no se recibe un ACK válido dentro del tiempo límite.
 */
bool waitForAck(byte expectedDeviceAddress) {

  unsigned long startTime = millis(); 
  while (millis() - startTime < 1000) { 
    if (CAN.checkReceive() == CAN_MSGAVAIL) {
      unsigned long id;
      byte len;
      byte buf[8];
      CAN.readMsgBuf(&id, &len, buf);
      if (id == 0x201 && len == 1) { // Verificar si es un ACK
        if (buf[0] == expectedDeviceAddress) {
          return true; 
        }
      }
    }
  }
  return false;

}


/**
 * @brief Envía un ACK (Acknowledgment) al receptor a través del bus CAN.
 * 
 * @param deviceAddress Dirección del dispositivo que realiza la operación.
 */
void sendAckToCan(byte deviceAddress) {
  byte ackData[1] = {deviceAddress}; // El ACK contiene la misma dirección

  if (CAN.sendMsgBuf(CAN_COMMAND_ACK, 0, 1, ackData) != CAN_OK) { // ID 0x201 para ACK
    Serial.println("Error enviando ACK.");
  }
}