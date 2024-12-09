/**
 * Author: Cristian Marrero Vega
 * Date: 23/11/2024
 * Installed Lybraries: mcp_can.
 * Description: Este programa permite la comunicación con el bus CAN mediante 
 * el controlador MCP2515 y la interacción con el usuario a través de comandos 
 * enviados por el puerto serie. Se basa en la librería mcp_can para manejar 
 * el controlador CAN.
 */

#include <SPI.h>
#include "mcp_can.h"

#define MAX_COMMAND_SIZE 50 // Tamaño máximo del comando

const int SPI_CS_PIN = 3; // Pin CS del MCP2515
MCP_CAN CAN(SPI_CS_PIN);

#define MAX_COMMAND_SIZE 50 // Tamaño máximo del comando

char inputBuffer[MAX_COMMAND_SIZE]; // Buffer para el comando
int bufferIndex = 0;                // Índice del buffer

void setup(){
  // Inicia la comunicación serie
  Serial.begin(9600);
  Serial.println(F("Listo para recibir comandos..."));

  // Inicializa el MCP2515 con una velocidad de 500 kbps
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println(F("MCP2515 Iniciado correctamente"));
  } else {
    Serial.println(F("Error al iniciar el MCP2515"));
    while (1); // Detener ejecución en caso de error
  }

  CAN.setMode(MCP_NORMAL); // Modo normal para transmisión
  Serial.println(F("Modo CAN: Normal"));
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



