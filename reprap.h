#ifndef _REPRAP_H_
#define _REPRAP_H_

#include "comms.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

// Must be called once before any library use. If return value is
// nonzero, init has failed and the library is unusable.
int rr_init();

// Should be called after library is no longer needed and all ports
// have been closed.
void rr_exit();

#ifdef __cplusplus
}
#endif

#endif
