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

#define DEV_PATH "/dev/"
#define DEV_PREFIXES ((char*[]){"ttyUSB", "ttyACM", NULL})
#define GUESSES 8

rr_port *rr_enumerate_ports() {
  size_t size = 4, fill = 0;
  rr_port *ports = calloc(size, sizeof(rr_port));
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

  if(fill >= size) {
    ++size;
    ports = realloc(ports, size * sizeof(char*));
  }
  ports[fill] = NULL;
  
  return ports;
}
