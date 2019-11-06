#include "cnes_6502.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deassembly.h"

static uint16_t  getloc(CNESCPU* c, opcode code);

#define GETSTATUS(c) \
c->s = (c->N<<7)+(c->V<<6)+(c->B<<4)+(c->D<<3)+(c->I<<2)+(c->Z<<1)+c->C

#define SETSTATUS(c,v) \
c->N = v>>7&0x1;\
c->V = v>>6&0x1;\
c->B = v>>4&0x1;\
c->D = v>>3&0x1;\
c->I = v>>2&0x1;\
c->Z = v>>1&0x1;\
c->C = v&0x1;\

#define SETCFLAGS(c, tmp) \
if((tmp) > 0xff)(c)->C = 1; \
else (c)->C = 0;

#define SETCFLAG(c, bl) \
if((bl) )(c)->C = 1; \
else (c)->C = 0;

#define SETZFLAGS(c,tmp) \
if(((tmp)&0xff)==0)(c)->Z = 1; \
else (c)->Z = 0;

#define SETVFLAGS(c,tmp) \
if(tmp) (c)->V = 1; \
else (c)->V = 0;

#define SETNFLAGS(c,tmp) \
if((tmp)&0x80) (c)->N = 1; \
else (c)->N = 0;


static inline uint8_t fetch(CNESCPU* c, opcode code) {
	if (code.am == am_imm) {
		return cnes_cpu_imm(c);
	}
	else if (code.am == imp) {
		return 0;
	}
	else {
		uint16_t loc = getloc(c, code);
		c->opaddr = loc;
		return cnes_bus_cpu_read(c->bus, loc);
	}
}

static inline void push(CNESCPU* c, uint8_t val) {
	cnes_bus_cpu_write(c->bus, c->sp, val);
	c->sp--;
}
static inline uint8_t pull(CNESCPU* c) {
	uint8_t val = cnes_bus_cpu_read(c->bus, c->sp);
	c->sp++;
	return val;
}

CNESCPU* cnes_cpu_alloc() {
	CNESCPU *cpu = (CNESCPU*)malloc(sizeof(*cpu));
	memset(cpu, 0, sizeof(*cpu));
	return cpu;
}
void cnes_cpu_free(CNESCPU *cpu) {
	cpu->bus = NULL;
	free(cpu);
}

void cnes_cpu_connect_bus(CNESCPU* cpu, CNESBUS* bus)
{
	cpu->bus = bus;
}

uint8_t cnes_cpu_imm(CNESCPU* cpu)
{
	uint8_t val = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	printf("#$%2X", val);
	return val;
}

uint16_t cnes_cpu_abs(CNESCPU* cpu)
{
	uint16_t loc = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	loc += cnes_bus_cpu_read(cpu->bus, cpu->pc++) << 8;
	printf("$%02X%02X", loc & 0xff, (loc >> 8) & 0xff);
	return loc;
}

uint16_t cnes_cpu_zp(CNESCPU* cpu)
{
	uint16_t loc = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	printf("$%02X", loc);
	return loc;
}

uint16_t cnes_cpu_zpx(CNESCPU* cpu)
{
	uint16_t loc = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	printf("$%02X,X", loc );
	loc = loc + cpu->x;
	return loc;
}

uint16_t cnes_cpu_zpy(CNESCPU* cpu)
{
	uint16_t loc = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	printf("$%02X,Y", loc);
	loc = loc + cpu->y;
	return loc;
}

uint16_t cnes_cpu_izx(CNESCPU* cpu)
{
	uint16_t loc = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	printf("($%02X,X)",loc);
	loc = loc + cpu->x;
	loc = cnes_bus_cpu_read(cpu->bus, loc);
	loc += cnes_bus_cpu_read(cpu->bus, loc + 1);
	return loc;
}

uint16_t cnes_cpu_izy(CNESCPU* cpu)
{
	uint16_t loc = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	printf("($%02X),Y",loc);
	loc = cnes_bus_cpu_read(cpu->bus, loc);
	loc += cnes_bus_cpu_read(cpu->bus, loc+1);
	loc = loc + cpu->y;
	return loc;
}


uint16_t cnes_cpu_abx(CNESCPU* cpu)
{
	uint16_t loc = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	loc += cnes_bus_cpu_read(cpu->bus, cpu->pc++) << 8;
	printf("$%02X%02X,X", (loc >> 8) & 0xff, loc & 0xff );
	loc += cpu->x;
	return loc;
}

uint16_t cnes_cpu_aby(CNESCPU* cpu)
{
	uint16_t loc = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	loc += cnes_bus_cpu_read(cpu->bus, cpu->pc++) << 8;
	printf("$%02X%02X,Y", (loc >> 8) & 0xff, loc & 0xff );
	loc += cpu->y;
	return loc;
}

uint16_t cnes_cpu_ind(CNESCPU* cpu)
{
	uint16_t loc = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	loc += cnes_bus_cpu_read(cpu->bus, cpu->pc++) << 8;
	printf("($%02X%02X)", (loc>>8) & 0xff, loc  & 0xff);
	loc = cnes_bus_cpu_read(cpu->bus, loc);
	loc += cnes_bus_cpu_read(cpu->bus, loc+1);
	return loc;
}

int8_t cnes_cpu_rel(CNESCPU* cpu)
{
	int8_t offset = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	printf("$%02X", offset);
	cpu->pc += offset;
	return offset;
}


static uint16_t  getloc(CNESCPU* c, opcode code) {
	switch (code.am)
	{
		//implied
	case imp:
		break;
		//immediate
	case am_imm:
		break;
	case am_zp:
		return cnes_cpu_zp(c);
	case am_zpx:
		return cnes_cpu_zpx(c);
	case am_zpy:
		return cnes_cpu_zpy(c);
	case am_izx:
		return cnes_cpu_izx(c);
	case am_izy:
		return cnes_cpu_izy(c);
	case am_abs:
		return cnes_cpu_abs(c);
	case am_abx:
		return cnes_cpu_abx(c);
	case am_aby:
		return cnes_cpu_aby(c);
	case am_ind:
		return cnes_cpu_ind(c);
	case am_rel:
		return cnes_cpu_rel(c);
	default:
		break;
	}
	return 0;
}
static void adc(CNESCPU* c,opcode code) {
	printf("ADC ");
	uint8_t val = fetch(c, code);
	uint16_t result = (uint16_t)c->a + (uint16_t)c->C + (uint16_t)val;

	SETCFLAGS(c, result);
	SETZFLAGS(c, result);
	SETNFLAGS(c, result);
	SETVFLAGS(c, ~((c->a ^ val) & 0x80) && ((c->a ^ result) & 0x80));
	c->a = (uint8_t)result;
}

static void ahx(CNESCPU* c, opcode code) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("AHX ");
}

static void anc(CNESCPU* c, opcode code) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("ANC ");
	uint8_t val = fetch(c, code);
	uint16_t tmp = (uint16_t)c->a & (uint16_t)val;
	SETNFLAGS(c, tmp);
	SETZFLAGS(c, tmp);
	SETCFLAGS(c, tmp);
	c->a = (uint8_t)tmp;
}

static void and (CNESCPU* c, opcode code) {
	printf("AND ");
	uint8_t val = fetch(c, code);
	uint16_t tmp = c->a & val;
	SETZFLAGS(c, tmp);
	SETNFLAGS(c, tmp);
	c->a = (uint8_t)tmp;
}

static void alr(CNESCPU* c, opcode code) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("ALR ");
}

static void arr(CNESCPU* c, opcode code) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("ARR ");
}

static void asl(CNESCPU* c, opcode code) {
	printf("ASL ");
	uint8_t src = fetch(c, code);
	if (src & 0x80) {
		c->C = 1;
	}
	else {
		c->C = 0;
	}

	src <<= 1;
	src &= 0xff;
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	if (code.am == imp) {
		c->a = src;
	}
	else {
		cnes_bus_cpu_write(c->bus, c->opaddr, src);
	}
}

static void axs(CNESCPU* c, opcode code) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("AXS ");
}

static void bcc(CNESCPU* c, opcode code) {
	printf("BCC ");
	if (!c->C) {
		cnes_cpu_rel(c);
	}
}

static void bcs(CNESCPU* c, opcode code) {
	printf("BCS ");
	if (c->C) {
		cnes_cpu_rel(c);
	}
}

static void beq(CNESCPU* c, opcode code) {
	printf("BEQ ");
	if (c->Z) {
		cnes_cpu_rel(c);
	}
}

static void bit(CNESCPU* c, opcode code) {
	printf("BIT");
	uint16_t src = fetch(c, code);
	SETNFLAGS(c, src);
	SETVFLAGS(c, 0x40 & src);
	if (src & c->a) {
		c->Z = 0;
	}
	else {
		c->Z = 1;
	}
}

static void bmi(CNESCPU* c, opcode op) {
	printf("BMI ");
	if (c->N) {
		cnes_cpu_rel(c);
	}
}

static void bne(CNESCPU* c, opcode op) {
	printf("BNE ");
	if (c->Z) {
		cnes_cpu_rel(c);
	}
	else {
		c->pc++;
	}
}

static void bpl(CNESCPU* c, opcode op) {
	printf("BPL ");
	if (!c->N) {
		cnes_cpu_rel(c);
	}
}

static void brk(CNESCPU* c, opcode op) {
	printf("BRK ");
	c->pc++;
	push(c, (c->pc) >> 8 & 0xff);
	push(c, c->pc & 0xff);
	c->I = 1;
	GETSTATUS(c);
	push(c, c->s);
	c->pc = cnes_bus_cpu_read(c->bus, 0xfffe);
	c->pc += cnes_bus_cpu_read(c->bus, 0xffff)<<8;
}

static void bvc(CNESCPU* c, opcode op) {
	printf("BVC ");
	if (!c->V) {
		c->pc = cnes_cpu_rel(c);
	}
}

static void bvs(CNESCPU* c, opcode op) {
	printf("BVS ");
	if (c->V) {
		c->pc = cnes_cpu_rel(c);
	}
}

static void clc(CNESCPU* c, opcode op) {
	printf("CLC ");
	c->C = 0;
}

static void cld(CNESCPU* c, opcode op) {
	printf("CLD ");
	c->D = 0;
}

static void cli(CNESCPU* c, opcode op) {
	printf("CLI ");
	c->I = 0;
}

static void clv(CNESCPU* c, opcode op) {
	printf("CLV ");
	c->V = 0;
}

static void cmp(CNESCPU* c, opcode op) {
	printf("CMP ");
	uint16_t src = fetch(c, op);
	src = (uint16_t)c->a - src;
	if (src < 0x100) {
		c->C = 1;
	}
	else
	{
		c->C = 0;
	}
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);

}

static void cpx(CNESCPU* c, opcode op) {
	printf("CPX ");
	uint16_t src = fetch(c, op);
	src = (uint16_t)c->x - src;
	SETCFLAG(c, src < 0x100);
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
}

static void cpy(CNESCPU* c, opcode op) {
	printf("CPY ");
	uint16_t src = fetch(c, op);
	src = (uint16_t)c->y - src;
	SETCFLAG(c, src < 0x100);
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
}

static void dcp(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("DCP ");
}

static void dec(CNESCPU* c, opcode op) {
	printf("DEC ");
	uint16_t src = fetch(c, op);
	src = (src - 1) & 0xff;
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	cnes_bus_cpu_write(c->bus, c->opaddr, src);
}

static void dex(CNESCPU* c, opcode op) {
	printf("DEX ");
	c->x--;
	uint8_t src = c->x;
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
}
static void dey(CNESCPU* c, opcode op) {
	printf("DEY ");
	c->y--;
	uint8_t src = c->y;
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
}

static void eor(CNESCPU* c, opcode op) {
	printf("EOR ");
	uint8_t src = fetch(c, op);
	src = c->a ^ src;
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	c->a = src;
}

static void inc(CNESCPU* c, opcode op) {
	printf("INC ");
	uint8_t src = fetch(c, op);
	src++;
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	cnes_bus_cpu_write(c->bus, c->opaddr, src);
}

static void inx(CNESCPU* c, opcode op) {
	printf("INX ");
	c->x++;
	SETNFLAGS(c, c->x);
	SETZFLAGS(c, c->x);
}

static void iny(CNESCPU* c, opcode op) {
	printf("INY ");
	c->y++;
	SETNFLAGS(c, c->y);
	SETZFLAGS(c, c->y);
}

static void isc(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("ISC ");
}

static void jmp(CNESCPU* c, opcode op) {
	printf("JMP ");
	uint16_t loc = getloc(c, op);
	c->pc = loc;
}

static void jsr(CNESCPU* c, opcode op) {
	printf("JSR ");
	uint16_t loc = getloc(c, op);
	c->pc--;
	push(c, c->pc >> 8);
	push(c, c->pc);
	c->pc = loc;
}

static void las(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("LAS ");
}

static void lax(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("LAX ");
}

static void lda(CNESCPU* c, opcode op) {
	printf("LDA ");
	uint8_t src = fetch(c, op);
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	c->a = src;
}

static void ldx(CNESCPU* c, opcode op) {
	printf("LDX ");
	uint8_t src = fetch(c, op);
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	c->x = src;
}

static void ldy(CNESCPU* c, opcode op) {
	printf("LDY ");
	uint8_t src = fetch(c, op);
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	c->y = src;
}

static void lsr(CNESCPU* c, opcode op) {
	printf("LSR ");
	uint8_t src = fetch(c, op);
	SETCFLAG(c, src & 0x01);
	src >>= 1;
	SETNFLAGS(c,src);
	SETZFLAGS(c,src);
	if (op.am == imp) {
		c->a = src;
	}
	else {
		cnes_bus_cpu_write(c->bus, c->opaddr, src);
	}
}

static void nop(CNESCPU* c, opcode op) {
	printf("NOP ");
}

static void ora(CNESCPU* c, opcode op) {
	printf("ORA ");
	uint8_t src = fetch(c, op);
	src |= c->a;
	SETNFLAGS(c,src);
	SETZFLAGS(c,src);
	c->a = src;
}

static void pha(CNESCPU* c, opcode op) {
	printf("PHA ");
	push(c, c->a);
}

static void php(CNESCPU* c, opcode op) {
	printf("PHP ");
	GETSTATUS(c);
	push(c, c->s);
}

static void pla(CNESCPU* c, opcode op) {
	printf("PLA ");
	uint8_t src = pull(c);
	SETNFLAGS(c,src);
	SETZFLAGS(c,src);
}

static void plp(CNESCPU* c, opcode op) {
	printf("PLP ");
	uint8_t src = pull(c);
	c->s = src;
	SETSTATUS(c, src);
}

static void rla(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("RLA ");
}

static void rol(CNESCPU* c, opcode op) {
	printf("ROL ");
	uint16_t src = fetch(c, op);
	src <<= 1;
	if (c->C) {
		src |= 0x1;
	}
	SETCFLAGS(c, src);
	src &= 0xff;
	SETNFLAGS(c,src);
	SETZFLAGS(c,src);
	if (op.am == imp) {
		c->a = src;
	}
	else {
		cnes_bus_cpu_write(c->bus, c->opaddr, src);
	}
}

static void ror(CNESCPU* c, opcode op) {
	printf("ROR ");
	uint16_t src = fetch(c, op);
	src <<= 1;
	if (c->C) {
		src |= 0x1;
	}
	SETCFLAGS(c, src);
	src &= 0xff;
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	if (op.am == imp) {
		c->a = src;
	}
	else {
		cnes_bus_cpu_write(c->bus, c->opaddr, src);
	}
}

static void rra(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("RRA ");
}

static void rti(CNESCPU* c, opcode op) {
	printf("RTI ");
	uint16_t src = pull(c);
	SETSTATUS(c, src);
	src = pull(c);
	src += pull(c) << 8;
	c->pc = src;
}

static void rts(CNESCPU* c, opcode op) {
	printf("RTS ");
	uint16_t src = pull(c);
	src += (pull(c) << 8) + 1;
	c->pc = src;
}

static void sax(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("SAX ");
}

static void sbc(CNESCPU* c, opcode op) {
	printf("SBC ");
	uint8_t src = fetch(c,op);
	unsigned int tmp = c->a-src;
	if (!c->C) {
		tmp--;
	}

	SETNFLAGS(c, tmp);
	SETZFLAGS(c, tmp);
	SETVFLAGS(c, ((c->a ^ tmp) & 0x80) && ((c->a ^ src) & 0x80));
	if (c->D) {
		if (((c->a & 0xf) - (c->C ? 0 : 1)) < (src & 0xf))
		{
			if (tmp > 0x99) {
				tmp -= 0x60;
			}
		}
	}
	SETCFLAG(c, tmp < 0x100);
	c->a = tmp & 0xff;
}

static void sec(CNESCPU* c, opcode op) {
	printf("SEC ");
	c->C = 1;
}

static void sed(CNESCPU* c, opcode op) {
	printf("SED ");
	c->D = 1;
}


static void sei(CNESCPU* c, opcode op) {
	printf("SEI ");
	c->I = 1;
}


static void shx(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("SHX ");
}

static void shy(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("SHY ");
}

static void slo(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("SLO ");
}

static void sre(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("SRE ");
}

static void sta(CNESCPU* c, opcode op) {
	printf("STA ");
	uint16_t loc = getloc(c, op);
	cnes_bus_cpu_write(c->bus, loc, c->a);
}

static void stp(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("STP ");
}

static void stx(CNESCPU* c, opcode op) {
	printf("STX ");
	uint16_t loc = getloc(c, op);
	cnes_bus_cpu_write(c->bus, loc, c->x);
}

static void sty(CNESCPU* c, opcode op) {
	printf("STY ");
	uint16_t loc = getloc(c, op);
	cnes_bus_cpu_write(c->bus, loc, c->y);
}

static void tas(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("TAS ");
}

static void tax(CNESCPU* c, opcode op) {
	printf("TAX ");
	uint8_t src = c->a;
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	c->x = src;
}

static void tay(CNESCPU* c, opcode op) {
	printf("TAY ");
	uint8_t src = c->a;
	SETNFLAGS(c, src);
	SETZFLAGS(c, src);
	c->y = src;
}

static void tsx(CNESCPU* c, opcode op) {
	printf("TSX ");
	SETNFLAGS(c,c->sp);
	SETZFLAGS(c, c->sp);
	c->x = c->sp;
}


static void txa(CNESCPU* c, opcode op) {
	printf("TXA ");
	SETNFLAGS(c, c->x);
	SETZFLAGS(c, c->x);
	c->a = c->x;
}

static void txs(CNESCPU* c, opcode op) {
	printf("TXS ");
	c->sp = c->x;
}

static void tya(CNESCPU* c, opcode op) {
	printf("TYA ");
	SETNFLAGS(c,c->y);
	SETZFLAGS(c,c->x);
	c->a = c->y;
}

static void xaa(CNESCPU* c, opcode op) {
	printf("$$$$NOT NES OPTION$$$$\n");
	printf("XAA ");
}

typedef void (*cnes_cpu_func)(CNESCPU* c, opcode op);

cnes_cpu_func funcs[] = {
	&adc,
	&ahx,
	&anc,
	&and,
	&alr,
	&arr,
	&asl,
	&axs,

	&bcc,
	&bcs,
	&beq,
	&bit,
	&bmi,
	&bne,
	&bpl,
	&brk,
	&bvc,
	&bvs,

	&clc,
	&cld,
	&cli,
	&clv,
	&cmp,
	&cpx,
	&cpy,

	&dcp,
	&dec,
	&dex,
	&dey,

	&eor,

	&inc,
	&inx,
	&iny,
	&isc,

	&jmp,
	&jsr,

	&las,
	&lax,
	&lda,
	&ldx,
	&ldy,
	&lsr,

	&nop,
	&ora,
	&pha,
	&php,
	&pla,
	&plp,

	&rla,
	&rol,
	&ror,
	&rra,
	&rti,
	&rts,

	&sax,
	&sbc,
	&sec,
	&sed,
	&sei,
	&shx,
	&shy,
	&slo,
	&sre,
	&sta,
	&stp,
	&stx,
	&sty,

	&tas,
	&tax,
	&tay,
	&tsx,
	&txa,
	&txs,
	&tya,
	&xaa,
};


void cnes_cpu_run_step(CNESCPU* cpu) {
	uint8_t val = cnes_bus_cpu_read(cpu->bus, cpu->pc++);
	opcode code = get_opcode(val);
	funcs[code.func](cpu, code);
	printf("\n");
}

int cnes_cpu_run(CNESCPU* cpu)
{
	cpu->sp = 0x100;
	cpu->pc = 0x600;
	while (1)
	{
		cnes_cpu_run_step(cpu);
	}
	return 0;
}
