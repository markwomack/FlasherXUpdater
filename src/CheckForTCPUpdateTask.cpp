//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

// My includes
#include <DebugMsgs.h>

// Local includes
#include "CheckForTCPUpdateTask.h"

CheckForTCPUpdateTask::CheckForTCPUpdateTask() : CheckForUpdateTask() {
  _tcpServer = 0;
}

void CheckForTCPUpdateTask::setTCPServer(WiFiServer* tcpServer) {
  _tcpServer = tcpServer;
}
    
void CheckForTCPUpdateTask::start(void) { 
  if (_tcpServer == 0) {
    DebugMsgs.debug().println("TCP Server not specified!");
  }
  
  CheckForUpdateTask::start();
}
    
void CheckForTCPUpdateTask::update(void) {
  WiFiClient tcpClient = _tcpServer->available();

  // If a client was returned, store it for future use
  // and indicate an update is now available
  if (tcpClient) {
    _tcpClient = tcpClient;
    _updateAvailable = true;
  }
}

Stream* CheckForTCPUpdateTask::getUpdateStream() {
  return (Stream*)&_tcpClient;
}
