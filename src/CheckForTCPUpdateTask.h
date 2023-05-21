//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef CHECKFORTCPUPDATETASK_H
#define CHECKFORTCPUPDATETASK_H

// Arduino includes
#include <Arduino.h>
#include <WiFiServer.h>
#include <WiFiClient.h>

// Local includes
#include "CheckForUpdateTask.h"

// This is a task that checks a TCP port for available
// data, and raises a flag to indicate that an update
// is available.
class CheckForTCPUpdateTask : public CheckForUpdateTask {
  public:
    CheckForTCPUpdateTask();
   
    void setTCPServer(WiFiServer* tcpServer);
    
    void start(void);
    void update(void);
    
    Stream* getUpdateStream();

  protected:
    WiFiServer* _tcpServer;
    WiFiClient _tcpClient;
};

#endif // CHECKFORTCPUPDATETASK_H
