#ifndef _XIOD_H
#define _XIOD_H

#include <stdint.h>
#include <stdio.h>

#define XIO_TCP 0x1;
#define XIO_DOMAIN_SOCKET 0x2;
#define XIO_SHM 0x4;

typedef struct {
  char* name;
  uint32_t transports;
  FILE* fd;
} xio_nd_rec;

#endif
