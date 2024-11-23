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

    

  } else {

    Serial.println(F("Comando no reconocido"));

  }

}


void us_processor(char* token){

  char* sensor_selct = token;

  /*
  Serial.print(F("US seleccionado "));
  Serial.println(sensor);
  */

  token = strtok(NULL, " ");

  if(strcmp(token, "one-shot") == 0){

    Serial.println(F(" one-shot"));

  }else if (strcmp(token, "off") == 0){

    Serial.println(F(" off"));


  }else if (strcmp(token, "status") == 0){

    Serial.println(F(" status"));

  }else if (strcmp(token, "status") == 0){
    //Serial.println(F(" mas parametros"));
    token = strtok(NULL, " ");

  }

}