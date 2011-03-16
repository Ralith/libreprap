#include "util.h"

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#ifdef USB
#include <libusb-1.0/libusb.h>
#endif

#include "ports.h"
#include "ports_private.h"
#include "reprap_private.h"

#define DEV_PATH "/dev/"
#define DEV_PREFIXES ((char*[]){"ttyUSB", "ttyACM", NULL})
#define GUESSES 8

/* TODO: Decide on some standard values for these */
#define RR_VENDOR 0x00
#define RR_PRODUCT 0xBEEF
#define RR_DEVICE_SUBCLASS 0x42

inline int is_usb_reprap(struct libusb_device_descriptor d) {
  return
    d.bDeviceClass == LIBUSB_CLASS_VENDOR_SPEC &&
    d.bDeviceSubClass == RR_DEVICE_SUBCLASS &&
    d.idVendor == RR_VENDOR &&
    d.idProduct == RR_PRODUCT;
}

rr_port *rr_enumerate_ports() {
  size_t size = 4, fill = 0;
  rr_port *ports = calloc(size, sizeof(rr_port));

  /* Search for USB ports */
  {
    libusb_device **devs;
    ssize_t len = libusb_get_device_list(usbctx, &devs);
    if(len >= 0) {
      size_t i;
      for(i = 0; i < (size_t)len; ++i) {
        struct libusb_device_descriptor desc;
        libusb_get_device_descriptor(devs[i], &desc);

        if(is_usb_reprap(desc)) {
          
          if(fill >= size) {
            size *= 2;
            ports = realloc(ports, size * sizeof(rr_port));
          }
          ports[fill] = malloc(sizeof(rr_port_t));
          
          ports[fill]->type = PORT_USB;

          const size_t namemax = 256;
          ports[fill]->name = malloc(namemax);
          libusb_device_handle *handle;
          libusb_open(devs[i], &handle);
          libusb_get_string_descriptor_ascii(handle, desc.iProduct, (unsigned char*)ports[fill]->name, namemax);
          libusb_close(handle);


          ports[fill]->usb.device = devs[i];
          ports[fill]->usb.handle = NULL;          
          ++fill;
        }
      }

      libusb_free_device_list(devs, 0);
    }    
  }

  /* Search for serial ports */
  {
    DIR *devdir = opendir(DEV_PATH);
    if(!devdir) {
      return NULL;
    }
  
    struct dirent *file;
    while((file = readdir(devdir))) {
      size_t i;
      for(i = 0; DEV_PREFIXES[i]; ++i) {
        char *prefix = DEV_PREFIXES[i];
        if(!strncmp(file->d_name, prefix, strlen(prefix))) {
          /* TODO: Open connection and interrogate device */
          if(fill >= size) {
            size *= 2;
            ports = realloc(ports, size * sizeof(rr_port));
          }
          ports[fill] = malloc(sizeof(rr_port_t));
          ports[fill]->type = PORT_SERIAL;
          const size_t namelen = strlen(file->d_name + 1);
          ports[fill]->name = malloc(namelen);
          ports[fill]->serial.path = malloc(namelen + strlen(DEV_PATH) + 1);
          ports[fill]->serial.baud = 0;
          strcpy(ports[fill]->name, file->d_name);
          strcpy(ports[fill]->serial.path, DEV_PATH);
          strcat(ports[fill]->serial.path, file->d_name);
          ++fill;
        }
      }
    }
    closedir(devdir);
  }

  if(fill >= size) {
    ++size;
    ports = realloc(ports, size * sizeof(char*));
  }
  ports[fill] = NULL;
  
  return ports;
}
