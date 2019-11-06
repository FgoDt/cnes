#ifndef _DEASM_H_
#define _DEASM_H_
#include <stdint.h>

typedef enum add_mode
{
	imp,
	am_imm, // #$00
	am_zp, //$00
	am_zpx,// $00,x
	am_zpy, // $00,y
	am_izx, //($00,x)
	am_izy, // ($00),y
	am_abs, // $0000
	am_abx, //$0000, x
	am_aby, //$0000,y
	am_ind, //($0000)
	am_rel //$0000
}add_mode;

typedef struct opcode
{
    enum opfunc func;
    add_mode am;
	int cycle;
	int add_one_cycle;
    int addr1;
    int addr2;
}opcode;


typedef enum opfunc
{
	CNES_ADC,
	CNES_AHX,
	CNES_ANC,
	CNES_AND,
	CNES_ALR,
	CNES_ARR,
	CNES_ASL,
	CNES_AXS,

	CNES_BCC,
	CNES_BCS,
	CNES_BEQ,
	CNES_BIT,
	CNES_BMI,
	CNES_BNE,
	CNES_BPL,
	CNES_BRK,
	CNES_BVC,
	CNES_BVS,

	CNES_CLC,
	CNES_CLD,
	CNES_CLI,
	CNES_CLV,
	CNES_CMP,
	CNES_CPX,
	CNES_CPY,

	CNES_DCP,
	CNES_DEC,
	CNES_DEX,
	CNES_DEY,

	CNES_EOR,

	CNES_INC,
	CNES_INX,
	CNES_INY,
	CNES_ISC,


	CNES_JMP,
	CNES_JSR,

	CNES_LAS,
	CNES_LAX,
	CNES_LDA,
	CNES_LDX,
	CNES_LDY,
	CNES_LSR,

	CNES_NOP,

	CNES_ORA,

	CNES_PHA,
	CNES_PHP,
	CNES_PLA,
	CNES_PLP,

	CNES_RLA,
	CNES_ROL,
	CNES_ROR,
	CNES_RRA,
	CNES_RTI,
	CNES_RTS,

	CNES_SAX,
	CNES_SBC,
	CNES_SEC,
	CNES_SED,
	CNES_SEI,
	CNES_SHX,
	CNES_SHY,
	CNES_SLO,
	CNES_SRE,
	CNES_STA,
	CNES_STP,
	CNES_STX,
	CNES_STY,

	CNES_TAS,
	CNES_TAX,
	CNES_TAY,
	CNES_TSX,
	CNES_TXA,
	CNES_TXS,
	CNES_TYA,
	CNES_XAA,
}opfunc;




opcode get_opcode(uint8_t op);

#endif