//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#include <DebugMsgs.h>

#include "FXUtil.h"     // https://github.com/joepasquariello/FlasherX
extern "C" {
  #include "FlashTxx.h"    // TLC/T3x/T4x/TMM flash primitives
}

#include "FlasherXUpdater.h"

void FlasherXUpdater::performUpdate(Stream* updateStream) {

  CRCStream* crcStream = getCRCStream(updateStream);
  if (!crcStream) {
    return;
  }
  
  uint32_t buffer_addr;
  uint32_t buffer_size;

  // create flash buffer to hold new firmware
  if (firmware_buffer_init( &buffer_addr, &buffer_size ) == 0) {
    DebugMsgs.debug().println( "unable to create buffer" );
    DebugMsgs.flush();
    free(crcStream);
    return;
  }
  
  DebugMsgs.debug().printfln( "created buffer = %1luK %s (%08lX - %08lX)",
   buffer_size/1024, IN_FLASH(buffer_addr) ? "FLASH" : "RAM",
    buffer_addr, buffer_addr + buffer_size );

  update_firmware(crcStream, (Stream*)&Serial, buffer_addr, buffer_size);
  
  // return from update_firmware() means error, so clean up and
  // reboot to ensure that static vars get boot-up initialized before retry
  DebugMsgs.debug().println( "ABORT - NO UPDATE, erasing FLASH buffer / free RAM buffer...restarting in 5 seconds" );
  firmware_buffer_free( buffer_addr, buffer_size );
  DebugMsgs.flush();
  delay(5000);
  REBOOT;
}

CRCStream* FlasherXUpdater::getCRCStream(Stream* updateStream) {    
  uint32_t expectedSize = 0;
  uint32_t expectedCRC = 0;
  
  uint8_t buffer[25];
  int count = 0;
  while (updateStream->available()) {
    int data = updateStream->read();
    if (data != -1) {
      if (data == '!') {
        buffer[count] = 0;
        expectedSize = strtoul((const char*)buffer, 0, 10);
        break;
      } else {
        buffer[count++] = (uint8_t)data;
      }
    }
  }

  if (expectedSize == 0) {
    DebugMsgs.debug().println("Error reading the expected size, aborted");
    return 0;
  }

  count = 0;
  while (updateStream->available()) {
    count += updateStream->readBytes(buffer + count, 9 - count);
    if (count == 9) {
      if (buffer[8] == '!') {
        buffer[8] = 0;
        expectedCRC = strtoul((const char*)buffer, 0, 16);
      }
      break;
    }
  }

  if (expectedCRC == 0) {
    DebugMsgs.error().println("Error reading the expected crc, aborted");
    return 0;
  }

  DebugMsgs.debug().printfln("Expected update size: %d, expected update CRC: %x", expectedSize, expectedCRC);
  
  return new CRCStream(updateStream, expectedSize, expectedCRC);
}
