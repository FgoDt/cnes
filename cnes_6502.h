#ifndef _CNES_6502_H_
#define _CNES_6502_H_
#include <stdint.h>
#include "cnes_bus.h"

typedef struct cnes_cpu{
	CNESBUS* bus;
	uint8_t a;// a register
	uint8_t x;// x register
	uint8_t y;// y register

	uint8_t s;// status register
	uint8_t N;
	uint8_t V;
	uint8_t B;
	uint8_t D;
	uint8_t I;
	uint8_t Z;
	uint8_t C;

	uint16_t pc;// program counter
	uint16_t sp;// stack pointer
	uint16_t opaddr;//opcode addr
}CNESCPU;

CNESCPU* cnes_cpu_alloc();
void cnes_cpu_free(CNESCPU *cpu);
void cnes_cpu_connect_bus(CNESCPU* cpu, CNESBUS* bus);

uint8_t cnes_cpu_imm(CNESCPU* cpu);
uint16_t cnes_cpu_zp(CNESCPU* cpu);
uint16_t cnes_cpu_zpx(CNESCPU* cpu);
uint16_t cnes_cpu_zpy(CNESCPU* cpu);
uint16_t cnes_cpu_izx(CNESCPU* cpu);
uint16_t cnes_cpu_izy(CNESCPU* cpu);
uint16_t cnes_cpu_abs(CNESCPU* cpu);
uint16_t cnes_cpu_abx(CNESCPU* cpu);
uint16_t cnes_cpu_aby(CNESCPU* cpu);
uint16_t cnes_cpu_ind(CNESCPU* cpu);
int8_t cnes_cpu_rel(CNESCPU* cpu);

int cnes_cpu_run(CNESCPU *cpu);

#endif