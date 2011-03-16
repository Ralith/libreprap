#include "reprap.h"
#include "reprap_private.h"

libusb_context *usbctx;

void rr_init() {
#ifdef USB
  libusb_init(&usbctx);
#endif
}

void rr_exit() {
#ifdef USB
  libusb_exit(usbctx);
#endif
}
