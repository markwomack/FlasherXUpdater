//******************************************************************************
// FXUTIL.H -- FlasherX utility functions
//******************************************************************************
#ifndef FXUTIL_H_
#define FXUTIL_H_

#include <Stream.h>
#include "CRCStream.h"

void set_read_timeout(uint32_t timeoutInMs);
void update_firmware( CRCStream *in, uint32_t buffer_addr, uint32_t buffer_size );

#endif
