//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef CHECKFORUPDATETASK_H
#define CHECKFORUPDATETASK_H

#include <Task.h>
#include <Stream.h>

class CheckForUpdateTask : public Task {
  public:
    CheckForUpdateTask();
    
    virtual void start(void);
    
    // Subclass must override
    virtual void update(void);

    bool updateIsAvailable();
    
    // Subclass must override
    virtual Stream* getUpdateStream();
    
  protected:
    bool _updateAvailable;
};

#endif // CHECKFORUPDATETASK_H
