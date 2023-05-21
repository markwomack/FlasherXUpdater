//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef CHECKFORSERIALUPDATETASK_H
#define CHECKFORSERIALUPDATETASK_H

#include <HardwareSerial.h>
#include "CheckForUpdateTask.h"

class CheckForSerialUpdateTask : public CheckForUpdateTask {
  public:
    CheckForSerialUpdateTask();
    CheckForSerialUpdateTask(HardwareSerial* hardwareSerial);
    
    void update(void);

    Stream* getUpdateStream();
    
  protected:
    Stream* _serialStream;
};

#endif // CHECKFORSERIALUPDATETASK_H
