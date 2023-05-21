//******************************************************************************
// FXUTIL.H -- FlasherX utility functions
//******************************************************************************
#include <Arduino.h>
#include <DebugMsgs.h>
#include "CRCStream.h"
extern "C" {
  #include "FlashTxx.h"		// TLC/T3x/T4x/TMM flash primitives
}

//******************************************************************************
// hex_info_t	struct for hex record and hex file info
//******************************************************************************
typedef struct {	// 
  char *data;		// pointer to array allocated elsewhere
  unsigned int addr;	// address in intel hex record
  unsigned int code;	// intel hex record type (0=data, etc.)
  unsigned int num;	// number of data bytes in intel hex record
 
  uint32_t base;	// base address to be added to intel hex 16-bit addr
  uint32_t min;		// min address in hex file
  uint32_t max;		// max address in hex file
  
  int eof;		// set true on intel hex EOF (code = 1)
  int lines;		// number of hex records received  
} hex_info_t;

// Default timeout for character read is 10ms
uint32_t timeout = 10;

//******************************************************************************
// defined methods
//******************************************************************************
void set_read_timeout(uint32_t timeoutInMs);
void read_ascii_line( Stream *serial, char *line, int maxbytes,
         bool* streamHasCRLF );
int  parse_hex_line( const char *theline, char *bytes,
	unsigned int *addr, unsigned int *num, unsigned int *code );
int  process_hex_record( hex_info_t *hex );
void update_firmware( Stream *in, uint32_t buffer_addr, uint32_t buffer_size );

//******************************************************************************
// update_firmware()	read hex file and write new firmware to program flash
//******************************************************************************
void update_firmware( CRCStream *in, uint32_t buffer_addr, uint32_t buffer_size )
{
  static char line[96];		                // buffer for hex lines
  static char data[32] __attribute__ ((aligned (8))); // buffer for hex data
  hex_info_t hex = {					// intel hex info struct
    data, 0, 0, 0,					//   data,addr,num,code
    0, 0xFFFFFFFF, 0, 					//   base,min,max,
    0, 0						//   eof,lines
  };

  DebugMsgs.debug().println( "reading hex lines..." );

  bool streamHasCRLF = false;
  
  // read and process intel hex lines until EOF or error
  while (!hex.eof)  {

    read_ascii_line( in, line, sizeof(line), &streamHasCRLF);

    if (parse_hex_line( (const char*)line, hex.data, &hex.addr, &hex.num, &hex.code ) == 0) {
      DebugMsgs.debug().printfln( "ABORT - bad hex line '%s', length: %d", line, strlen(line) );
      return;
    }
    else if (process_hex_record( &hex ) != 0) { // error on bad hex code
      DebugMsgs.debug().printfln( "ABORT - invalid hex code %d", hex.code );
      return;
    }
    else if (hex.code == 0) { // if data record
      uint32_t addr = buffer_addr + hex.base + hex.addr - FLASH_BASE_ADDR;
      if (hex.max > (FLASH_BASE_ADDR + buffer_size)) {
        DebugMsgs.debug().printfln( "ABORT - max address %08lX too large", hex.max );
        return;
      }
      else if (!IN_FLASH(buffer_addr)) {
        memcpy( (void*)addr, (void*)hex.data, hex.num );
      }
      else if (IN_FLASH(buffer_addr)) {
        int error = flash_write_block( addr, hex.data, hex.num );
        if (error) {
          DebugMsgs.debug().printfln( "ABORT - error %02X in flash_write_block()", error );
	        return;
        }
      }
    }
    hex.lines++;
  }
  
  if (!in->sizeAndCRCMatch()) {
    DebugMsgs.debug().println( "abort - data size or CRC does not match expected" );
    DebugMsgs.debug().printfln( "Expected size: %d, actual: %d", in->getExpectedSize(), in->getCurrentSize());
    DebugMsgs.debug().printfln( "Expected CRC: %x, actual: %x", in->getExpectedCRC(), in->getCurrentCRC());
    return;
  }

  DebugMsgs.debug().printfln( "data size: %d, data CRC: %x", in->getCurrentSize(), in->getCurrentCRC());
  DebugMsgs.debug().printfln( "hex file: %1d lines %1lu bytes (%08lX - %08lX)",
			hex.lines, hex.max-hex.min, hex.min, hex.max );

  // check FSEC value in new code -- abort if incorrect
  #if defined(KINETISK) || defined(KINETISL)
  uint32_t value = *(uint32_t *)(0x40C + buffer_addr);
  if (value == 0xfffff9de) {
    DebugMsgs.debug().printfln( "new code contains correct FSEC value %08lX", value );
  }
  else {
    DebugMsgs.debug().printfln( "ABORT - FSEC value %08lX should be FFFFF9DE", value );
    return;
  } 
  #endif

  // check FLASH_ID in new code - abort if not found
  if (check_flash_id( buffer_addr, hex.max - hex.min )) {
    DebugMsgs.debug().printfln( "new code contains correct target ID %s", FLASH_ID );
  }
  else {
    DebugMsgs.debug().printfln( "ABORT - new code missing string %s", FLASH_ID );
    return;
  }

// We don't want user input for our purposes
//  // get user input to write to flash or abort
//  int user_lines = -1;
//  while (user_lines != hex.lines && user_lines != 0) {
//    out->printf( "enter %d to flash or 0 to abort\n", hex.lines );
//    read_ascii_line( out, line, sizeof(line) );
//    sscanf( line, "%d", &user_lines );
//  }
//  
//  if (user_lines == 0) {
//    out->printf( "ABORT - user entered 0 lines\n" );
//    return;
//  }
//  else {
    DebugMsgs.debug().println( "SUCCESS - calling flash_move() to load new firmware in 5 seconds...\n" );
    DebugMsgs.flush();
    delay(5000);
//  }
  
  // move new program from buffer to flash, free buffer, and reboot
  flash_move( FLASH_BASE_ADDR, buffer_addr, hex.max-hex.min );

  // should not return from flash_move(), but put REBOOT here as reminder
  REBOOT;
}

//******************************************************************************
// read_ascii_line()	read ascii characters until '\n', '\r', or max bytes
//******************************************************************************
void read_ascii_line( Stream *serial, char *line, int maxbytes, bool* streamHasCRLF )
{
  int c;
  int count = 0;
  int crlfCount = 0;
  uint32_t lastReadTime = millis();
  while(true) {
    if (count >= (maxbytes - 1)) {
      DebugMsgs.debug().printfln("ERROR - Max bytes read: %s, %d", line, strlen(line));
      break;
    }
    if (millis() >= (lastReadTime + timeout)) {
      DebugMsgs.debug().printfln("ERROR - Timeout (%d) exceeded: %s, %d", timeout, line, strlen(line));
      break;
    }
    if (serial->available()) {
      c = serial->read();
      lastReadTime = millis();
    } else {
      continue;
    }
    
    // if c is a CR or LF
    if (c == '\r' || c == '\n') {
      if (count == 0) {
        // use this as indication that lines have CRLF
        *streamHasCRLF = true;
      } else {
        if (*streamHasCRLF) {
          crlfCount++;
          if (crlfCount == 2) {
            // reached end of line
            break;
          }
        } else {
          // reached end of line
          break;
        }
      }
    } else {
      line[count++] = c;
    }
  }
  
  // null terminate
  line[count] = 0;
}

//******************************************************************************
// process_hex_record()		process record and return okay (0) or error (1)
//******************************************************************************
int process_hex_record( hex_info_t *hex )
{
  if (hex->code==0) { // data -- update min/max address so far
    if (hex->base + hex->addr + hex->num > hex->max)
      hex->max = hex->base + hex->addr + hex->num;
    if (hex->base + hex->addr < hex->min)
      hex->min = hex->base + hex->addr;
  }
  else if (hex->code==1) { // EOF (:flash command not received yet)
    hex->eof = 1;
  }
  else if (hex->code==2) { // extended segment address (top 16 of 24-bit addr)
    hex->base = ((hex->data[0] << 8) | hex->data[1]) << 4;
  }
  else if (hex->code==3) { // start segment address (80x86 real mode only)
    return 1;
  }
  else if (hex->code==4) { // extended linear address (top 16 of 32-bit addr)
    hex->base = ((hex->data[0] << 8) | hex->data[1]) << 16;
  }
  else if (hex->code==5) { // start linear address (32-bit big endian addr)
    hex->base = (hex->data[0] << 24) | (hex->data[1] << 16)
              | (hex->data[2] <<  8) | (hex->data[3] <<  0);
  }
  else {
    return 1;
  }

  return 0;
}

//******************************************************************************
// Intel Hex record foramt:
//
// Start code:  one character, ASCII colon ':'.
// Byte count:  two hex digits, number of bytes (hex digit pairs) in data field.
// Address:     four hex digits
// Record type: two hex digits, 00 to 05, defining the meaning of the data field.
// Data:        n bytes of data represented by 2n hex digits.
// Checksum:    two hex digits, computed value used to verify record has no errors.
//
// Examples:
//  :10 9D30 00 711F0000AD38000005390000F5460000 35
//  :04 9D40 00 01480000 D6
//  :00 0000 01 FF
//******************************************************************************

/* Intel HEX read/write functions, Paul Stoffregen, paul@ece.orst.edu */
/* This code is in the public domain.  Please retain my name and */
/* email address in distributed copies, and let me know about any bugs */

/* I, Paul Stoffregen, give no warranty, expressed or implied for */
/* this software and/or documentation provided, including, without */
/* limitation, warranty of merchantability and fitness for a */
/* particular purpose. */

// type modifications by Jon Zeeff

/* parses a line of intel hex code, stores the data in bytes[] */
/* and the beginning address in addr, and returns a 1 if the */
/* line was valid, or a 0 if an error occured.  The variable */
/* num gets the number of bytes that were stored into bytes[] */

#include <stdio.h>		// sscanf(), etc.
#include <string.h>		// strlen(), etc.

int parse_hex_line( const char *theline, char *bytes, 
		unsigned int *addr, unsigned int *num, unsigned int *code )
{
  unsigned sum, len, cksum;
  const char *ptr;
  int temp;

  *num = 0;
  if (theline[0] != ':')
    return 0;
  if (strlen (theline) < 11)
    return 0;
  ptr = theline + 1;
  if (!sscanf (ptr, "%02x", &len))
    return 0;
  ptr += 2;
  if (strlen (theline) < (11 + (len * 2)))
    return 0;
  if (!sscanf (ptr, "%04x", (unsigned int *)addr))
    return 0;
  ptr += 4;
  /* Serial.printf("Line: length=%d Addr=%d\n", len, *addr); */
  if (!sscanf (ptr, "%02x", code))
    return 0;
  ptr += 2;
  sum = (len & 255) + ((*addr >> 8) & 255) + (*addr & 255) + (*code & 255);
  while (*num != len)
  {
    if (!sscanf (ptr, "%02x", &temp))
      return 0;
    bytes[*num] = temp;
    ptr += 2;
    sum += bytes[*num] & 255;
    (*num)++;
    if (*num >= 256)
      return 0;
  }
  if (!sscanf (ptr, "%02x", &cksum))
    return 0;

  if (((sum & 255) + (cksum & 255)) & 255)
    return 0;     /* checksum error */
  return 1;
}

void set_read_timeout(uint32_t timeoutInMs) {
  timeout = timeoutInMs;
}
