//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef TCPTOSERIALTASK_H
#define TCPTOSERIALTASK_H

#include <DebugMsgs.h>
#include <NetworkServer.h>
#include <NetworkClient.h>
#include <Task.h>

class TCPToSerialTask : public Task {
  public:
    TCPToSerialTask() {
      _hasTCPClient = false;
    };
    
    void setTCPServer(NetworkServer* tcpServer) {
      _tcpServer = tcpServer;
    };

    void start(void) {
      _hasTCPClient = false;
    };
    
    void update(void) {
      if (!_hasTCPClient) {
        NetworkClient tcpClient = _tcpServer->available();
        if (tcpClient) {
          DebugMsgs.debug().println("New TCP update, sending over serial");
          DebugMsgs.flush();
          _tcpClient = tcpClient;
          _hasTCPClient = true;
          _totalSize = 0;
        }
      }

      if (_hasTCPClient) {
        uint8_t buffer[512];

        if (_tcpClient.available()) {
          int size = _tcpClient.readBytes(buffer, 512);
          _totalSize += Serial5.write(buffer, size);
        } else {
          _hasTCPClient = false;
          _tcpClient.stop();
          DebugMsgs.debug().printfln("Total bytes sent: %d", _totalSize);
          DebugMsgs.flush();
        }
      }
    }

  private:
    NetworkServer* _tcpServer;
    NetworkClient _tcpClient;
    bool _hasTCPClient;
    uint32_t _totalSize;
};

#endif // TCPTOSERIALTASK_H
