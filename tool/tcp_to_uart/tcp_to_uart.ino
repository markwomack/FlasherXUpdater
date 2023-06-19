//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

// This is a sketch that will recieve data via a TCP port
// and redirect it to a UART port. It can be used in
// conjuction with the uart_update example.

// Arduino includes
#include <Arduino.h>

// My includes
#include <DebugMsgs.h>       // https://github.com/markwomack/ArduinoLogging
#include <TaskManager.h>     // https://github.com/markwomack/TaskManager
#include <WiFiNetworkHub.h>  // https://github.com/markwomack/TeensyNetworkHub
#include <NetworkServer.h>

// Local includes
#include "secrets.h"
#include "pin_assignments.h"
#include "TCPToSerialTask.h"

// Fixed IP address for this sketch
const IPAddress HOST_IP_ADDRESS(IPAddress(192, 168, 86, 101));

// TCP port to receive data on
const uint32_t TCP_SERVER_PORT(55555); 

WiFiNetworkHub networkHub = WiFiNetworkHub::getInstance();

#define SERIAL_BUFFER_SIZE 16384
uint8_t incomingBuffer[SERIAL_BUFFER_SIZE];
uint8_t outgoingBuffer[SERIAL_BUFFER_SIZE];

TCPToSerialTask tcpToSerialTask;

void setup() {
  Serial.begin(115200);
  Serial5.begin(115200);
  Serial5.addMemoryForRead(incomingBuffer, SERIAL_BUFFER_SIZE);
  Serial5.addMemoryForWrite(outgoingBuffer, SERIAL_BUFFER_SIZE);
  delay(500);

  DebugMsgs.enableLevel(ALL);

  pinMode(LED_STATUS_PIN, OUTPUT);

  // Connect to WiFi network, create a TCP port to monitor
  networkHub.setPins(SPI_MOSI_PIN, SPI_MISO_PIN, SPI_SCK_PIN, SPI_CS_PIN, RESET_PIN, BUSY_PIN);
  networkHub.setLocalIPAddress(HOST_IP_ADDRESS);
  if (networkHub.begin(SECRET_SSID, SECRET_PASS, DebugMsgs.getPrint())) {
    NetworkServer* tcpServer = networkHub.getServer(TCP_SERVER_PORT);
    tcpServer->begin();
    tcpToSerialTask.setTCPServer(tcpServer);
  }

  taskManager.addTask(&tcpToSerialTask, 5);
  taskManager.addBlinkTask(LED_STATUS_PIN, 500);
  taskManager.start();
}

void loop() {
  taskManager.update();
}
