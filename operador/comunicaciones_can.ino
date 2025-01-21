#define CAN_COMMAND_ID 0x200
#define CAN_COMMAND_ACK 0x201

/**
 * @brief Envía un ACK (Acknowledgment) al transmisor a través del bus CAN.
 * 
 * @param deviceAddress Dirección del dispositivo que realiza la operación.
 */
void sendAckToCan(byte deviceAddress) {
  byte ackData[1] = {deviceAddress}; // El ACK contiene la misma dirección

  if (CAN.sendMsgBuf(CAN_COMMAND_ACK, 0, 1, ackData) != CAN_OK) { // ID 0x201 para ACK
    Serial.println("Error enviando ACK.");
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


void send_sensor_status(Sensor& sensor) {
    byte packet[8]; // Paquete de datos CAN

    // Fragmento 1: Dirección, unidad y estado de disparo
    packet[0] = 0; // Índice 0
    packet[1] = sensor.address;
    packet[2] = sensor.unit;
    packet[3] = sensor.shotting ? 1 : 0;
    for (int i = 4; i < 8; i++) {
      packet[i] = 0x00; // Relleno
    }
    sendPacket(packet, CAN_COMMAND_ID);

    // Fragmento 2: min_delay
    packet[0] = 1; // Índice 1
    packet[1] = (sensor.min_delay >> 24) & 0xFF; // Byte más significativo
    packet[2] = (sensor.min_delay >> 16) & 0xFF;
    packet[3] = (sensor.min_delay >> 8) & 0xFF;
    packet[4] = sensor.min_delay & 0xFF;         // Byte menos significativo
    for (int i = 5; i < 8; i++) {
        packet[i] = 0x00; // Relleno
    }
    sendPacket(packet, CAN_COMMAND_ID);

    // Fragmento 3: period_delay
    packet[0] = 2; // Índice 2
    packet[1] = (sensor.period_delay >> 24) & 0xFF; // Byte más significativo
    packet[2] = (sensor.period_delay >> 16) & 0xFF;
    packet[3] = (sensor.period_delay >> 8) & 0xFF;
    packet[4] = sensor.period_delay & 0xFF;         // Byte menos significativo
    for (int i = 5; i < 8; i++) {
        packet[i] = 0x00; // Relleno
    }
    sendPacket(packet, CAN_COMMAND_ID);

    // Fragmento 4: Nombre del sensor
    int nameLength = sensor.name.length();
    for (int i = 0; i < nameLength; i += 7) {
        packet[0] = (i / 7) + 3; // Índice del fragmento
        for (int j = 0; j < 7; j++) {
            packet[j + 1] = (i + j < nameLength) ? sensor.name[i + j] : 0x00;
        }
        sendPacket(packet, CAN_COMMAND_ID);
    }

    // Fragmento 5: Última medición
    int lastShotLength = sensor.last_shot.length();
    for (int i = 0; i < lastShotLength; i += 7) {
        packet[0] = (i / 7) + 10; // Índice del fragmento (diferente bloque)
        for (int j = 0; j < 7; j++) {
            packet[j + 1] = (i + j < lastShotLength) ? sensor.last_shot[i + j] : 0x00;
        }
        sendPacket(packet, CAN_COMMAND_ID);
    }

    // Enviar paquete final con bit de fin
    packet[0] = 255; // Índice especial para indicar el fin
    for (int i = 1; i < 8; i++) {
        packet[i] = 0x00; // Relleno
    }
    sendPacket(packet, CAN_COMMAND_ID);
}


void sendPacket(byte* packet, unsigned long id) {
    if (CAN.sendMsgBuf(id, 0, 8, packet) != CAN_OK) {
        Serial.println("Error enviando paquete.");
    }
}



