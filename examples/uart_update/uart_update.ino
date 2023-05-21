//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

// This is an example that demostrates perfoming a Teensy firmware
// update from data that is provided through a UART port.

// Arduino includes
#include <Arduino.h>

// My includes
#include <DebugMsgs.h>       // https://github.com/markwomack/ArduinoLogging
#include <TaskManager.h>     // https://github.com/markwomack/TaskManager

#include <CheckForSerialUpdateTask.h>
#include <FlasherXUpdater.h>

#define SERIAL_BUFFER_SIZE 8192
uint8_t incomingBuffer[SERIAL_BUFFER_SIZE];
uint8_t outgoingBuffer[SERIAL_BUFFER_SIZE];

// This is the task that will monitor a UART port for a firmware update
CheckForSerialUpdateTask checkForSerialUpdateTask(&Serial5);

void setup() {
  Serial.begin(115200);
  Serial5.begin(115200);
  Serial5.addMemoryForRead(incomingBuffer, SERIAL_BUFFER_SIZE);
  Serial5.addMemoryForWrite(outgoingBuffer, SERIAL_BUFFER_SIZE);
  delay(500);
  
  DebugMsgs.enableLevel(ALL);
  
  // Configure the task manager to execute tasks
  
  // This task will check for a firmware update half second
  taskManager.addTask(&checkForSerialUpdateTask, 500);

  // This LED will blink (half second) during normal operations of the sketch
  taskManager.addBlinkTask(500);
  
  // Start normal operations
  taskManager.start();
}

void loop() {
  // Normal operation
  taskManager.update();

  // If there is a firmware update, stop everything and process
  if (checkForSerialUpdateTask.updateIsAvailable()) {
    DebugMsgs.debug().println("Firmware update available, processing...");

    // stop normal operation
    taskManager.stop();

    // perform the update
    FlasherXUpdater::performUpdate(checkForSerialUpdateTask.getUpdateStream());

    // update was aborted before a restart was required and the program can recover
    // restart task manager
    DebugMsgs.debug().println("Update was aborted, restarting normal operation");
    taskManager.start();
  }
}
