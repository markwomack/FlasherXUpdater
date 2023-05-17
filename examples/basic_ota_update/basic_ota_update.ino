//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

// Arduino includes
#include <Arduino.h>
#include <WiFiServer.h>
#include <WiFiClient.h>

// My includes
#include <DebugMsgs.h>       // https://github.com/markwomack/ArduinoLogging
#include <TaskManager.h>     // https://github.com/markwomack/TaskManager
#include <WiFiNetworkHub.h>  // https://github.com/markwomack/WiFiNetworkHub

#include "src/CheckForTCPUpdateTask.h"
#include "src/FlasherXUpdater.h"

// Local includes
#include "secrets.h"
#include "pin_assignments.h"

// This is the TCP port number monitored for a firmware update
const uint32_t TCP_SERVER_PORT(55555);

// This is the fixed IP address for this sketch
const IPAddress HOST_IP_ADDRESS(IPAddress(192, 168, 86, 101));

// This maintains the connection to the WiFi
WiFiNetworkHub networkHub;

// This is the task that will monitor a TCP port for a firmware update
CheckForTCPUpdateTask checkForTCPUpdateTask;

void setup() {
  Serial.begin(115200);
  delay(500);
  
  DebugMsgs.enableLevel(ALL);
  
  // Configure and start the WiFi network hub
  networkHub.setPins(SPI_MOSI_PIN, SPI_MISO_PIN, SPI_SCK_PIN, SPI_CS_PIN, RESET_PIN, BUSY_PIN);
  networkHub.setHostIPAddress(HOST_IP_ADDRESS);
  if (!networkHub.start(SECRET_SSID, SECRET_PASS, DebugMsgs.getPrint())) {
    DebugMsgs.error().println("Abort - error starting WiFi network hub");
    while(true){;}
  }
  networkHub.printWiFiStatus(DebugMsgs.getPrint());

  // Set the tcp port to be monitored into the checkForTCPUpdateTask
  checkForTCPUpdateTask.setTCPServer(networkHub.getTCPServer(TCP_SERVER_PORT));
  
  // Configure the task manager to execute tasks
  
  // This task will check for a firmware update every second
  taskManager.addTask(&checkForTCPUpdateTask, 1000);

  // This LED will blink (half second) during normal operations of the sketch
  taskManager.addBlinkTask(LED_STATUS_PIN, 500);
  
  // Start normal operations
  taskManager.start();
}

void loop() {
  // Normal operation
  taskManager.update();

  // If there is a firmware update, stop everything and process
  if (checkForTCPUpdateTask.updateIsAvailable()) {
    DebugMsgs.debug().println("Firmware update available, processing...");

    // stop normal operation
    taskManager.stop();

    // perform the update
    FlasherXUpdater::performUpdate(checkForTCPUpdateTask.getTCPClient());

    // update did not happen, and program can recover, restart task manager
    taskManager.start();
  }
}
