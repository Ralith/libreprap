#ifndef _PORTS_H_
#define _PORTS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rr_port_t *rr_port;

rr_port rr_port_serial(const char *name, const char *path, unsigned long speed);

// Necessary to set baudrate of enumerated serial ports
void rr_port_seral_set_speed(rr_port port, unsigned long speed);

const char *rr_port_name(const rr_port port);

int rr_port_open(rr_port port);
int rr_port_close(rr_port port);

void rr_port_free(rr_port port);

#ifdef __cplusplus
}
#endif

#endif
