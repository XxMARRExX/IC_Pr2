#include <SPI.h>
#include "mcp_can.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "sensors.h"
#include "timers.h"
#include "oled_display.h"

const int SPI_CS_PIN = 3; // Pin CS del MCP2515
MCP_CAN CAN(SPI_CS_PIN);


Sensor* sensors[] = {&sensor1, &sensor2, &oled};

union valor {
    byte bytes[4];
    unsigned long value;
} periodo;

void setup() {
  Serial.begin(9600);
    while (!Serial) {
        ;
    }

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // No continuar si hay fallo
  }

  Wire.begin();
  delay(100);

  init_sensors();
  init_TCC0();       // Inicializar temporizador para el sensor 1
  init_TC4_TC5();     // Inicializar temporizador para el sensor 2

  //enable_tcc0_interrupt(); // Habilitar interrupciones para TCC0 (sensor 1)
  //enable_tc4_tc5_interrupt(); // Habilitar interrupciones para TC4 (sensor 2)

  drawOLED(); // Mostrar información inicial en OLED

  
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
  
  checkInterrupts();
  
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

/**
 * @brief Verifica si hay interrupciones pendientes y las procesa.
 */
void checkInterrupts() {
  if (flag1){
    flag1 = 0;
    shot(sensor1);
  }

  if (flag2){
    flag2 = 0;
    shot(sensor2);
  }
}


/**
 * @brief Envía un ACK (Acknowledgment) al transmisor a través del bus CAN.
 * 
 * @param deviceAddress Dirección del dispositivo que realiza la operación.
 */
void sendAckToCan(byte deviceAddress) {
  byte ackData[1] = {deviceAddress}; // El ACK contiene la misma dirección

  if (CAN.sendMsgBuf(0x201, 0, 1, ackData) != CAN_OK) { // ID 0x201 para ACK
    Serial.println("Error enviando ACK.");
  }
}


/**
 * @brief Devuelve un puntero al sensor correspondiente según su dirección.
 * 
 * Esta función permite acceder al sensor especificado mediante su dirección
 * en la lista de sensores configurados. Si la dirección no es válida, se devuelve `NULL`.
 * 
 * @param deviceAddress Dirección del sensor (0x00, 0x01, 0x02).
 * @return Sensor* Puntero al sensor correspondiente, o `NULL` si la dirección no es válida.
 */
Sensor* getSensor(byte deviceAddress) {
  if (deviceAddress < 3) {
    return sensors[deviceAddress];
  }
  return NULL;
}



