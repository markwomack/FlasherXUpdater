//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

// This is an example that demostrates perfoming a Teensy firmware
// update from data that is provided through a TCP port.

// This define is picked up in the Arduino includes
#define SERIAL_BUFFER_SIZE 8192

// Arduino includes
#include <Arduino.h>

// My includes
#include <DebugMsgs.h>       // https://github.com/markwomack/ArduinoLogging
#include <TaskManager.h>     // https://github.com/markwomack/TaskManager

//#include <CheckForUSBUpdateTask.h>
//#include <FlasherXUpdater.h>
#include "src/CheckForUSBUpdateTask.h"
#include "src/FlasherXUpdater.h"

// This is the task that will monitor Serial for a firmware update
CheckForUSBUpdateTask checkForUSBUpdateTask;

void setup() {
  Serial.begin(115200);
  delay(500);
  
  DebugMsgs.enableLevel(ALL);
  
  // Configure the task manager to execute tasks
  
  // This task will check for a firmware update half second
  taskManager.addTask(&checkForUSBUpdateTask, 500);

  // This LED will blink (half second) during normal operations of the sketch
  taskManager.addBlinkTask(500);
  
  // Start normal operations
  taskManager.start();
}

void loop() {
  // Normal operation
  taskManager.update();

  // If there is a firmware update, stop everything and process
  if (checkForUSBUpdateTask.updateIsAvailable()) {
    DebugMsgs.debug().println("Firmware update available, processing...");

    // stop normal operation
    taskManager.stop();

    // perform the update
    FlasherXUpdater::performUpdate(&Serial);

    // update was aborted before a restart was required and the program can recover
    // restart task manager
    taskManager.start();
  }
}
