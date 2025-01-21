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


void sendSensorRequest(byte deviceAddress) {
  byte data[8] = {0x30, deviceAddress}; // Comando 0x30 para solicitar estado

  // Enviar solicitud al esclavo
  if (CAN.sendMsgBuf(CAN_COMMAND_ID, 0, 8, data) == CAN_OK) {
      if (!waitForAck(deviceAddress)) {
          Serial.println("Error: No se recibió ACK del dispositivo.");
          return;
      }
  } else {
      Serial.println("Error enviando mensaje.");
      return;
  }

  // Recibir y procesar los datos del sensor
  receiveSensorStatus();
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


void receiveSensorStatus() {
  byte buffer[8];         // Buffer para almacenar los fragmentos
  Sensor receivedSensor;  // Estructura para almacenar los datos recibidos
  String receivedName = "";
  String receivedLastShot = "";
  bool finished = false;  // Indica si se ha recibido el paquete final

  while (!finished) {
    if (CAN.checkReceive() == CAN_MSGAVAIL) {
      unsigned long id;
      byte len;
      CAN.readMsgBuf(&id, &len, buffer);

      switch (buffer[0]) {
        case 0: // Fragmento 1: Dirección, unidad y estado de disparo
          receivedSensor.address = buffer[1];
          receivedSensor.unit = buffer[2];
          receivedSensor.shotting = buffer[3] == 1;
          Serial.println("Fragmento 1 recibido:");
          Serial.print("Dirección: ");
          Serial.println(receivedSensor.address, HEX);
          Serial.print("Unidad: ");
          Serial.println(receivedSensor.unit);
          Serial.print("Estado: ");
          Serial.println(receivedSensor.shotting ? "Activado" : "Desactivado");
          break;

        case 1: // Fragmento 2: min_delay
          receivedSensor.min_delay = ((unsigned long)buffer[1] << 24) |
                                     ((unsigned long)buffer[2] << 16) |
                                     ((unsigned long)buffer[3] << 8) |
                                     (unsigned long)buffer[4];
          Serial.println("Fragmento 2 recibido:");
          Serial.print("Min delay: ");
          Serial.println(receivedSensor.min_delay);
          break;

        case 2: // Fragmento 3: period_delay
          receivedSensor.period_delay = ((unsigned long)buffer[1] << 24) |
                                        ((unsigned long)buffer[2] << 16) |
                                        ((unsigned long)buffer[3] << 8) |
                                        (unsigned long)buffer[4];
          Serial.println("Fragmento 3 recibido:");
          Serial.print("Period delay: ");
          Serial.println(receivedSensor.period_delay);
          break;

        default:
          if (buffer[0] < 10) { // Fragmentos del nombre del sensor
            Serial.println("Fragmento del nombre recibido:");
            for (int i = 1; i < 8; i++) {
              if (buffer[i] != 0x00) {
                receivedName += (char)buffer[i];
              }
            }
            Serial.print("Nombre parcial: ");
            Serial.println(receivedName);
          } else if (buffer[0] < 255) { // Fragmentos de la última medición
            Serial.println("Fragmento de la última medición recibido:");
            for (int i = 1; i < 8; i++) {
              if (buffer[i] != 0x00) {
                receivedLastShot += (char)buffer[i];
              }
            }
            Serial.print("Última medición parcial: ");
            Serial.println(receivedLastShot);
          } else if (buffer[0] == 255) { // Paquete final
            Serial.println("Paquete final recibido. Envío completo.");
            receivedSensor.name = receivedName;
            receivedSensor.last_shot = receivedLastShot;
            finished = true;
          }
          break;
      }
    }
  }

  // Imprime la información completa del sensor
  printSensorStatus(receivedSensor);
}


void printSensorStatus(Sensor& sensor) {
    Serial.println("\n--- Información completa del sensor ---");
    Serial.print("Nombre: ");
    Serial.println(sensor.name);
    Serial.print("Dirección: ");
    Serial.println(sensor.address, HEX);
    Serial.print("Unidad: ");
    Serial.println(sensor.unit);
    Serial.print("Min delay: ");
    Serial.println(sensor.min_delay);
    Serial.print("Period delay: ");
    Serial.println(sensor.period_delay);
    Serial.print("Estado: ");
    Serial.println(sensor.shotting ? "Activado" : "Desactivado");
    Serial.print("Última medición: ");
    Serial.println(sensor.last_shot);
    Serial.println("--------------------------------------");
}

