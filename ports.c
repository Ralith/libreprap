#include "ports_private.h"
#include "ports.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "serial.h"

rr_port rr_port_serial(const char *path, unsigned long speed) {
  rr_port ret = malloc(sizeof(rr_port_t));
  ret->type = PORT_SERIAL;
  ret->serial.path = malloc(strlen(path)+1);
  strcpy(ret->serial.path, path);
  ret->serial.baud = speed;
  ret->serial.fd = -1;
  return ret;
}

const char *rr_port_name(const rr_port port) {
  return port->name;
}

void rr_port_seral_set_speed(rr_port port, unsigned long speed) {
  if(port->type == PORT_SERIAL) {
    port->serial.baud = speed;
  }
}

int rr_port_open(rr_port port) {
  int result;
  switch(port->type) {
  case PORT_SERIAL:
    result = serial_open(port->serial.path, port->serial.baud);
    if(port->serial.fd >= 0) {
      port->serial.fd = result;
    }
    break;
  case PORT_USB: {
    result = libusb_open(port->usb.device, &port->usb.handle);
    break;
  }
  }
  return 0;
}

int rr_port_close(rr_port port) {
  int result;
  switch(port->type) {
  case PORT_SERIAL:
    do {
      result = close(port->serial.fd);
    } while(result < 0 && errno == EINTR);
    if(result < 0) {
      return result;
    }
    break;

  case PORT_USB:
    libusb_close(port->usb.handle);
    break;
  }
  
  return 0;
}

void rr_port_free(rr_port port) {
  switch(port->type) {
  case PORT_SERIAL:
    if(port->serial.fd >= 0) {
      close(port->serial.fd);
    }
    free(port->serial.path);
    break;
#ifdef USB
  case PORT_USB:
    if(port->usb.handle) {
      libusb_close(port->usb.handle);
    }
    libusb_unref_device(port->usb.device);
    break;
#endif
  }
  free(port);
}
