/**
 * Author: Cristian Marrero Vega
 * Date: 23/11/2024
 */

#define MAX_COMMAND_SIZE 50 // Tamaño máximo del comando

char inputBuffer[MAX_COMMAND_SIZE]; // Buffer para el comando
int bufferIndex = 0;                // Índice del buffer

void setup(){
  // Inicia la comunicación serie
  Serial.begin(9600);
  Serial.println(F("Listo para recibir comandos..."));
}

void loop(){
  
  if (Serial.available()) {
    char receivedChar = Serial.read();
    
    if (receivedChar == '\n') {
      inputBuffer[bufferIndex] = '\0';
      processCommand(inputBuffer);
      bufferIndex = 0;
    } else if (bufferIndex < MAX_COMMAND_SIZE - 1) {
      inputBuffer[bufferIndex++] = receivedChar;
    } else {
      Serial.println(F("Error: Comando demasiado largo."));
      bufferIndex = 0;
    }
  }

}