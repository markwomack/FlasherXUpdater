//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef CHECKFORTCPUPDATETASK_H
#define CHECKFORTCPUPDATETASK_H

// Arduino includes
#include <Arduino.h>
#include <NetworkServer.h>
#include <NetworkClient.h>

// Local includes
#include "CheckForUpdateTask.h"

// This is a task that checks a TCP port for available
// data, and raises a flag to indicate that an update
// is available.
class CheckForTCPUpdateTask : public CheckForUpdateTask {
  public:
    CheckForTCPUpdateTask();
   
    void setTCPServer(NetworkServer* tcpServer);
    
    void start(void);
    void update(void);
    
    Stream* getUpdateStream();

  protected:
    NetworkServer* _tcpServer;
    NetworkClient _tcpClient;
};

#endif // CHECKFORTCPUPDATETASK_H
