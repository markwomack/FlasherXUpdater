//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

// My includes
#include <DebugMsgs.h>

// Local includes
#include "CheckForTCPUpdateTask.h"

CheckForTCPUpdateTask::CheckForTCPUpdateTask() {
  _tcpServer = 0;
  _updateIsAvailable = false;
}

void CheckForTCPUpdateTask::setTCPServer(WiFiServer* tcpServer) {
  _tcpServer = tcpServer;
}

WiFiClient* CheckForTCPUpdateTask::getTCPClient() {
  return &_tcpClient;
}
    
void CheckForTCPUpdateTask::start(void) { 
  if (_tcpServer == 0) {
    DebugMsgs.debug().println("TCP Server not specified!");
  }
  
  _updateIsAvailable = false;
}
    
void CheckForTCPUpdateTask::update(void) {
  WiFiClient tcpClient = _tcpServer->available();

  // If a client was returned, store it for future use
  // and indicate an update is now available
  if (tcpClient) {
    _tcpClient = tcpClient;
    _updateIsAvailable = true;
  }
}

bool CheckForTCPUpdateTask::updateIsAvailable() {
  return _updateIsAvailable;
}
