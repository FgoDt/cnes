#ifndef _CNES_BUS_H_
#define _CNES_BUS_H_

#include <stdint.h>

typedef struct cnes_bus {
	uint8_t* mem;
}CNESBUS;

CNESBUS* cnes_bus_create();

void cnes_bus_destroy(CNESBUS* bus);

uint8_t cnes_bus_cpu_read(CNESBUS* bus,uint16_t loc);
int cnes_bus_cpu_write(CNESBUS* bus,uint16_t loc,uint8_t val);

#endif // !_CNES_BUS_H_
