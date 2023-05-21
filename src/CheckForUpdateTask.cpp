//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#include "CheckForUpdateTask.h"

CheckForUpdateTask::CheckForUpdateTask() {
  _updateAvailable = false;
}

void CheckForUpdateTask::start(void) {
  _updateAvailable = false;
}

void CheckForUpdateTask::update(void) {
  // this version does nothing
}

bool CheckForUpdateTask::updateIsAvailable() {
  return _updateAvailable;
}

Stream* CheckForUpdateTask::getUpdateStream() {
  // this version returns null
  return 0;
}

