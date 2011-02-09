#include "ports_private.h"
#include "ports.h"

#include <stdlib.h>
#include <string.h>

rr_port rr_port_serial(const char *path, unsigned long speed) {
  rr_port ret = malloc(sizeof(rr_port_t));
  ret->type = PORT_SERIAL;
  ret->serial.path = malloc(strlen(path)+1);
  strcpy(ret->serial.path, path);
  ret->serial.baud = speed;
  return ret
}

const char *rr_port_name(const rr_port port) {
  return port->name;
}

void rr_port_free(rr_port port) {
  switch(port->type) {
  case PORT_SERIAL:
    free(ret->serial.path);
    break;
#ifdef USB
  case PORT_USB:
    libusb_unref_device(ret->usb);
    break;
#endif
  }
  free(port);
}
