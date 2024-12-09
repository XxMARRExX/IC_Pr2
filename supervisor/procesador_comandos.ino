#define dir1  0xFF // Tamaño máximo del comando
#define us1   0x00 // Sensor ultrasonido1
#define us2   0x01 // Sensor ultrasonido2
#define oled  0x02 // Pantalla oled

char* option[2]; 


/**
 * @brief Procesa un comando recibido desde el puerto serie.
 * 
 * Divide el comando en tokens y ejecuta acciones específicas 
 * según el comando (e.g., `us`, `help`).
 * 
 * @param command Comando completo recibido desde el puerto serie.
 */
void processCommand(char* command) {
  Serial.print(F("Comando recibido: "));
  Serial.println(command);
  
  // Divide el comando en tokens usando espacio como delimitador
  char* token = strtok(command, " ");

  // 1. Comando vacío
  if (token == NULL) {
    Serial.println(F("Comando vacío"));
    return;
  }

  // 1. Comando us
  if (strcmp(token, "us") == 0) {

    token = strtok(NULL, " ");
    // 2. Variaciones comando us
    if(token != NULL){
      us_processor(token);
    }else{
      // 3. Comando us
      Serial.println(F("us."));
    }

  // 4. Comando help
  } else if (strcmp(token, "help") == 0) {
    help();
  } else {
    // 5. Comando no reconocido
    Serial.println(F("Comando no reconocido"));
  }

}


/**
 * @brief Procesa subcomandos para los sensores ultrasónicos.
 * 
 * Determina qué sensor (`us1` o `us2`) está seleccionado
 * y ejecuta acciones como `one-shot`, `on`, `off`, `unit`, `delay`, o `status`.
 * 
 * @param token Token que identifica el sensor y el subcomando.
 * @return int Retorna -1 si ocurre un error, 0 si el comando se procesa correctamente.
 */
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

    token = strtok(NULL, " ");
    int valor = check_time_processor(token);

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


/**
 * @brief Procesa valores de tiempo en segundos, minutos o horas.
 * 
 * Convierte el tiempo proporcionado en milisegundos según 
 * el modificador (`-s`, `-m`, `-h`).
 * 
 * @param token Modificador de tiempo (`-s`, `-m`, `-h`).
 * @return int Tiempo en milisegundos, o -1 si ocurre un error.
 */
int check_time_processor(char* token) {
    ...
}

int check_time_processor(char* token){

  char* valor = strtok(NULL, " ");
  int numero;

  if (valor != NULL && esNumeroValido(valor)) {
    numero = atoi(valor);
  } else {
    Serial.println("Error: Valor no es un número válido.");
    return -1;
  }


  if (strcmp(token, "-s") == 0){
    if(numero > 86400 || numero < 0){
      Serial.println(F("Valor fuera de rango para los segundos."));
    }
    return numero * 1000;
  }else if (strcmp(token, "-m") == 0){
    if(numero > 1440 || numero < 0){
      Serial.println(F("Valor fuera de rango para los minutis."));
    }
    return numero * 1000 * 60;
  }else if (strcmp(token, "-h") == 0){
    if(numero > 24 || numero < 0){
      Serial.println(F("Valor fuera de rango para las horas."));
    }
    return numero * 1000 * 60 * 60;
  }

}


/**
 * @brief Verifica si una cadena contiene únicamente caracteres numéricos.
 * 
 * @param str Cadena a validar.
 * @return true Si la cadena contiene solo dígitos.
 * @return false Si la cadena contiene caracteres no numéricos.
 */
bool esNumeroValido(const char* str) {
  while (*str) {
    if (!isdigit(*str)) {
      return false;
    }
    str++;
  }
  return true;
}


/**
 * @brief Muestra los comandos disponibles en el puerto serie.
 * 
 * Imprime una lista de los comandos disponibles para 
 * que el usuario los pueda consultar.
 */
void help(){

  Serial.println(F(" --- Comandos disponibles --- "));
  Serial.println(F("us [us1|us2] [one-shot | on <period_ms> [h|m|s] | off]"));
  Serial.println(F("us [us1|us2] unit [inc | cm | ms]"));
  Serial.println(F("us [us1|us2] delay <period_ms> [h|m|s]"));
  Serial.println(F("us [us1|us2] status"));
  Serial.println(F("us"));
  //Serial.println(F("off oled"));
  //Serial.println(F("on oled"));
  Serial.println(F("help"));
}