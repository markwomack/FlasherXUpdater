//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#include "CheckForSerialUpdateTask.h"

CheckForSerialUpdateTask::CheckForSerialUpdateTask() {
  _serialStream = &Serial;
}

CheckForSerialUpdateTask::CheckForSerialUpdateTask(HardwareSerial* hardwareSerial) {
  _serialStream = (Stream*)hardwareSerial;
}

void CheckForSerialUpdateTask::update(void) {
  if (_serialStream->available()) {
    _updateAvailable = true;
  }
}

Stream* CheckForSerialUpdateTask::getUpdateStream() {
  return _serialStream;
}
