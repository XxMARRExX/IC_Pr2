void oneshot_to_can(byte deviceAddrss) {

  byte data[2] = {0x00, deviceAddrss};

  if (CAN.sendMsgBuf(0x200, 0, 2, data) == CAN_OK) { 

    //Serial.println("Mensaje enviado. Esperando ACK...");
    if (waitForAck(deviceAddrss)) {
      //Serial.println("Comunicación exitosa. ACK recibido.");
    } else {
      //Serial.println("Error: No se recibió ACK.");
    }
  } else {
    //Serial.println("Error enviando mensaje.");
  }
}


void us_off(byte deviceAddrss) {

  byte data[2] = {0x01, deviceAddrss};

  if (CAN.sendMsgBuf(0x200, 0, 2, data) == CAN_OK) { 

    //Serial.println("Mensaje enviado. Esperando ACK...");
    if (waitForAck(deviceAddrss)) {
      //Serial.println("Comunicación exitosa. ACK recibido.");
    } else {
      //Serial.println("Error: No se recibió ACK.");
    }
  } else {
    //Serial.println("Error enviando mensaje.");
  }
}

// No implementado
void us_on(byte deviceAddrss) {

  byte data[2] = {0x01, deviceAddrss};

  if (CAN.sendMsgBuf(0x200, 0, 2, data) == CAN_OK) { 

    //Serial.println("Mensaje enviado. Esperando ACK...");
    if (waitForAck(deviceAddrss)) {
      //Serial.println("Comunicación exitosa. ACK recibido.");
    } else {
      //Serial.println("Error: No se recibió ACK.");
    }
  } else {
    //Serial.println("Error enviando mensaje.");
  }
}


void us_unit(byte unit, byte deviceAddrss) {

  byte data[2] = {unit, deviceAddrss};

  if (CAN.sendMsgBuf(0x200, 0, 2, data) == CAN_OK) { 

    //Serial.println("Mensaje enviado. Esperando ACK...");
    if (waitForAck(deviceAddrss)) {
      //Serial.println("Comunicación exitosa. ACK recibido.");
    } else {
      //Serial.println("Error: No se recibió ACK.");
    }
  } else {
    //Serial.println("Error enviando mensaje.");
  }
}


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
