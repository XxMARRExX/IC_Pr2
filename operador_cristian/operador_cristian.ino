#include <SPI.h>
#include "mcp_can.h"

const int SPI_CS_PIN = 3; // Pin CS del MCP2515
MCP_CAN CAN(SPI_CS_PIN);

struct Sensor {
  String name;
  byte address;
  byte unit;
  int min_delay;
  int period_delay;
  bool shotting;
  String last_shot;
};

Sensor sensor1;
Sensor sensor2;
Sensor sensor3;

Sensor* sensors[] = {&sensor1, &sensor2, &sensor3};

union valor {
    byte bytes[4];
    unsigned long value;
} periodo;

void setup() {
  Serial.begin(9600);

  // Inicializa el bus CAN
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 Iniciado correctamente");
  } else {
    Serial.println("Error al iniciar el MCP2515");
    while (1);
  }

  CAN.setMode(MCP_NORMAL); // Modo normal para recepción
  Serial.println("Receptor listo para recibir mensajes.");
}

void loop() {
  unsigned long id;     // ID del mensaje
  byte len;             // Longitud de los datos
  byte buf[8];          // Buffer para los datos

  // Verificar si hay un mensaje disponible
  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    CAN.readMsgBuf(&id, &len, buf); // Leer el mensaje

    if (id == 0x200) { // Verificar ID y longitud esperada
      byte operation = buf[0];      // Primer byte (operación)
      byte deviceAddress = buf[1];  // Segundo byte (dirección)
      periodo.bytes[0] = buf[2];
      periodo.bytes[1] = buf[3];
      periodo.bytes[2] = buf[4];
      periodo.bytes[3] = buf[5];

      Sensor* selectedSensor = getSensor(deviceAddress);

      if (selectedSensor == NULL) {
        // Procesa el sensor
        Serial.println("Error: Dirección de sensor no válida.");
      } else {
        
        switch (operation) {
          case 0x00:
              shot(*selectedSensor);
              break;

          case 0x01:
              set_sensor_on_period(*selectedSensor, (int)periodo.value);
              break;

          case 0x02:
              set_sensor_off(*selectedSensor);
              break;

          case 0x10:
              unit_inches(*selectedSensor);
              break;
          
          case 0x11:
              unit_cm(*selectedSensor);
              break;

          case 0x12:
              unit_ms(*selectedSensor);
              break;

          case 0x20:
              set_sensor_delay(*selectedSensor, (int)periodo.value);
              break;

          case 0x30:
              //(*selectedSensor);
              break;

          case 0x40:
              //();
              break;

          default:
              Serial.println("Operación no reconocida.");
              break;
        }
        // Envía un ACK de vuelta al transmisor
        sendAckToCan(deviceAddress);
      }
    }
  }
}


void sendAckToCan(byte deviceAddress) {
  byte ackData[1] = {deviceAddress}; // El ACK contiene la misma dirección

  if (CAN.sendMsgBuf(0x201, 0, 1, ackData) == CAN_OK) { // ID 0x201 para ACK
    //Serial.println("ACK enviado al transmisor.");
  } else {
    Serial.println("Error enviando ACK.");
  }
}


Sensor* getSensor(byte deviceAddress) {
  if (deviceAddress < 3) {
  return sensors[deviceAddress];
  }
  return NULL;
}

