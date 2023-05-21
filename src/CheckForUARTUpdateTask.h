//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef CHECKFORUARTUPDATETASK_H
#define CHECKFORUARTUPDATETASK_H

#include <Task.h>
#include <HardwareSerial.h>

class CheckForUARTUpdateTask : public Task {
  public:
    void setUART(HardwareSerial* serial);
    HardwareSerial* getUART();
    
    void start(void);
    void update(void);

    bool updateIsAvailable();
    
  private:
    bool _updateAvailable;
    HardwareSerial* _serial;
};

#endif // CHECKFORUARTUPDATETASK_H
