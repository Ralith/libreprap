#ifndef _REPRAP_PRIVATE_H_
#define _REPRAP_PRIVATE_H_

#ifdef USB
#include <libusb-1.0/libusb.h>
#endif

#ifdef USB
extern libusb_context *usbctx;
#endif

#endif
