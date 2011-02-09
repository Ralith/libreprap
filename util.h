#ifndef _REPRAP_UTIL_H_
#define _REPRAP_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ports.h"

/* Returns a NULL-terminated array of ports */
rr_port *rr_enumerate_ports();

#ifdef __cplusplus
}
#endif

#endif
