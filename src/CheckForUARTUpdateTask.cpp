//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#include "CheckForUARTUpdateTask.h"

void CheckForUARTUpdateTask::setUART(HardwareSerial* serial) {
  _serial = serial;
}

HardwareSerial* CheckForUARTUpdateTask::getUART() {
  return _serial;
}

void CheckForUARTUpdateTask::start(void) {
  _updateAvailable = false;
}

void CheckForUARTUpdateTask::update(void) {
  if (_serial->available()) {
    _updateAvailable = true;
  }
}

bool CheckForUARTUpdateTask::updateIsAvailable() {
  return _updateAvailable;
}
