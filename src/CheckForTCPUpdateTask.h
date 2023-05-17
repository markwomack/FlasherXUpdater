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

// My includes
#include <Task.h>

// This is a task that checks a TCP port for available
// data, and raises a flag to indicate that an update
// is available.
class CheckForTCPUpdateTask : public Task {
  public:
    CheckForTCPUpdateTask();
    
    void setTCPServer(WiFiServer* tcpServer);
    WiFiClient* getTCPClient();
    
    void start(void);
    void update(void);

    bool updateIsAvailable();

  private:
    WiFiServer* _tcpServer;
    WiFiClient _tcpClient;
    bool _updateIsAvailable;
};

#endif // CHECKFORTCPUPDATETASK_H
