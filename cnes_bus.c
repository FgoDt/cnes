#include "cnes_bus.h"
#include <stdlib.h>
#include <string.h>

CNESBUS* cnes_bus_create()
{
	CNESBUS* bus = (CNESBUS*)malloc(sizeof(*bus));
	memset(bus, 0, sizeof(*bus));
	bus->mem = (uint8_t*)malloc(0xffff);
	memset(bus->mem, 0, 0xffff);
	return bus;
}

void cnes_bus_destroy(CNESBUS* bus)
{
	free(bus->mem);
	free(bus);
}

uint8_t cnes_bus_cpu_read(CNESBUS* bus, uint16_t loc)
{
	return bus->mem[loc];
}

int cnes_bus_cpu_write(CNESBUS* bus, uint16_t loc, uint8_t val)
{
	bus->mem[loc] = val;
	return 0;
}
