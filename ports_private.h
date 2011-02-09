#ifndef _PORTS_PRIVATE_H_
#define _PORTS_PRIVATE_H_

#ifdef USB
#include <libusb-1.0/libusb.h>
#endif

typedef enum {
  PORT_SERIAL,
#ifdef USB
  PORT_USB,
#endif
} rr_port_type;

typedef struct rr_port_t {
  rr_port_type type;
  char *name;
  union {
    struct {
      char *path;
      unsigned long baud;
    } serial;
#ifdef USB
    libusb_device *usb;
#endif
  };
} rr_port_t;

#endif
