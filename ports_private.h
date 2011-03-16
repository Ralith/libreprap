#ifndef _PORTS_PRIVATE_H_
#define _PORTS_PRIVATE_H_

#ifdef USB
#include <libusb-1.0/libusb.h>
#endif

#include "comms.h"

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
      int fd;
    } serial;
#ifdef USB
    struct {
      libusb_device *device;
      libusb_device_handle *handle;
      rr_usb_proto proto;
    } usb;
#endif
  };
} rr_port_t;

#endif
