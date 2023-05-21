//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef CHECKFORUSBUPDATETASK_H
#define CHECKFORUSBUPDATETASK_H

#include <Task.h>
#include <HardwareSerial.h>

class CheckForUSBUpdateTask : public Task {
  public:
    void start(void);
    void update(void);

    bool updateIsAvailable();
    
  private:
    bool _updateAvailable;
};

#endif // CHECKFORUSBUPDATETASK_H
