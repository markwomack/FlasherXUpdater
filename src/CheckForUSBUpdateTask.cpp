//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#include "CheckForUSBUpdateTask.h"

void CheckForUSBUpdateTask::start(void) {
  _updateAvailable = false;
}

void CheckForUSBUpdateTask::update(void) {
  if (Serial.available()) {
    _updateAvailable = true;
  }
}

bool CheckForUSBUpdateTask::updateIsAvailable() {
  return _updateAvailable;
}
