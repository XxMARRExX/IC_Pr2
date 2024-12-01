#define dir1  0xFF // Tamaño máximo del comando
#define us1   0x00 // Sensor ultrasonido1
#define us2   0x01 // Sensor ultrasonido2
#define oled  0x02 // Pantalla oled

void processCommand(char* command) {
  Serial.print(F("Comando recibido: "));
  Serial.println(command);
  
  // Divide el comando en tokens usando espacio como delimitador
  char* token = strtok(command, " ");

  if (token == NULL) {
    Serial.println(F("Comando vacío"));
    return;
  }

  // Comparar cadenas con strcmp
  if (strcmp(token, "us") == 0) {

    token = strtok(NULL, " ");
    if(token != NULL){
      us_processor(token);
    }else{
      Serial.println(F("us."));
    }

  } else if (strcmp(token, "help") == 0) {

    Serial.println(F(" help."));

  } else {

    Serial.println(F("Comando no reconocido"));

  }

}


int us_processor(char* token){

  // 1. Comprobación del sensor del comando
  char* sensor_selct = token;
  byte sensor = 0x00;

  if (strcmp(sensor_selct, "us1") == 0){
    sensor = us1;
  } else if(strcmp(sensor_selct, "us2") == 0){
    sensor = us2;
  }else {
    Serial.println(F("Sensor no incorrecto"));
    return -1;
  }

  token = strtok(NULL, " ");

  // 2. Función one-shot
  if(strcmp(token, "one-shot") == 0){
    oneshot_to_can(sensor);

  // 3. Función on
  }else if (strcmp(token, "on") == 0){

    Serial.println(F(" on."));
    token = strtok(NULL, " ");

  // 4. Función off
  }else if (strcmp(token, "off") == 0){
    us_off(sensor);

  // 5. Función unit
  }else if (strcmp(token, "unit") == 0){
    token = strtok(NULL, " ");
    if (strcmp(token, "inc") == 0){
      byte unit = 0x10;
      us_unit(unit, sensor);
    } else if(strcmp(token, "cm") == 0){
      byte unit = 0x11;
      us_unit(unit, sensor);
    }else if(strcmp(token, "ms") == 0){
      byte unit = 0x12;
      us_unit(unit, sensor);
    }else{
      Serial.println(F("Unidad no reconocida"));
    }

  // 6. Función delay
  }else if (strcmp(token, "delay") == 0){

    Serial.println(F(" delay."));
    token = strtok(NULL, " ");

  // 7. Función status
  }else if (strcmp(token, "status") == 0){

    Serial.println(F(" status."));

  }
  
}
