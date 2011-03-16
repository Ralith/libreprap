#ifndef _REPRAP_H_
#define _REPRAP_H_

#include "comms.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

// Must be called once before and after all libreprap use
// respectively.  All ports should be closed before exit.
void rr_init();
void rr_exit();

#ifdef __cplusplus
}
#endif

#endif
