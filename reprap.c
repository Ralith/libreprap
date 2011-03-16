#include "reprap.h"
#include "reprap_private.h"

libusb_context *usbctx;

int rr_init() {
#ifdef USB
  return libusb_init(&usbctx);
#endif
}

void rr_exit() {
#ifdef USB
  libusb_exit(usbctx);
#endif
}
