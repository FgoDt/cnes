#include "deassembly.h"


opcode instruction_set_tab[] = {
	{CNES_BRK,imp,7},	{CNES_ORA,am_izx,6},	{CNES_STP,0,0},	{CNES_SLO,am_izx,8},{CNES_NOP,am_zp,3},{CNES_ORA,am_zp,3},{CNES_ASL,am_zp,5},{CNES_SLO,am_zp,5},
	{CNES_PHP,imp,3},	{CNES_ORA,am_imm,2},	{CNES_ASL,imp,2},	{CNES_ANC,am_imm,2},{CNES_NOP,am_abs,4},{CNES_ORA,am_abs,4},{CNES_ASL,am_abs,6},{CNES_SLO,am_abs,6},
	{CNES_BPL,am_rel,2,1},{CNES_ORA,am_izy,5,1},{CNES_STP,0,0},	{CNES_SLO,am_izy,8},{CNES_NOP,am_zpx,4},{CNES_ORA,am_zpx,4},{CNES_ASL,am_zpx,6},{CNES_SLO,am_zpx,6},
	{CNES_CLC,imp,2},	{CNES_ORA,am_aby,4,1},{CNES_NOP,imp,2},	{CNES_SLO,am_aby,7},{CNES_NOP,am_abx,4,1},{CNES_ORA,am_abx,4,1},{CNES_ASL,am_abx,7},{CNES_SLO,am_abx,7},
	{CNES_JSR,am_abs,6},	{CNES_AND,am_izx,6},	{CNES_STP,0,0},	{CNES_RLA,am_izx,8},{CNES_BIT,am_zp,3},{CNES_AND,am_zp,3},{CNES_ROL,am_zp,5},{CNES_RLA,am_zp,5},
	{CNES_PLP,imp,4},	{CNES_AND,am_imm,2},	{CNES_ROL,imp,2},	{CNES_ANC,am_imm,2},{CNES_BIT,am_abs,4},{CNES_AND,am_abs,4},{CNES_ROL,am_abs,6},{CNES_RLA,am_abs,6},
	{CNES_BMI,am_rel,2,1},{CNES_AND,am_izy,5,1},{CNES_STP,0,0},	{CNES_RLA,am_izy,8},{CNES_NOP,am_zpx,4},{CNES_AND,am_zpx,4},{CNES_ROL,am_zpx,6},{CNES_RLA,am_zpx,6},
	{CNES_SEC,imp,2},	{CNES_AND,am_aby,4,1},{CNES_NOP,imp,2},	{CNES_RLA,am_aby,7},{CNES_NOP,am_abx,4,1},{CNES_AND,am_abx,4,1},{CNES_ROL,am_abx,7},{CNES_RLA,am_abx,7},
	{CNES_RTI,imp,6},	{CNES_EOR,am_izx,6},	{CNES_STP,0,0},	{CNES_SRE,am_izx,8},{CNES_NOP,am_zp,3},{CNES_EOR,am_zp,3},{CNES_LSR,am_zp,5},{CNES_SRE,am_zp,5},
	{CNES_PHA,imp,3},	{CNES_EOR,am_imm,2},	{CNES_LSR,imp,2},	{CNES_ALR,am_imm,2},{CNES_JMP,am_abs,3},{CNES_EOR,am_abs,4},{CNES_LSR,am_abs,6},{CNES_SRE,am_abs,6},
	{CNES_BVC,am_rel,2,1},{CNES_EOR,am_izy,5,1},{CNES_STP,0,0},	{CNES_SRE,am_izy,8},{CNES_NOP,am_zpx,4},{CNES_EOR,am_zpx,4},{CNES_LSR,am_zpx,6},{CNES_SRE,am_zpx,6},
	{CNES_CLI,imp,2},	{CNES_EOR,am_aby,4,1},{CNES_NOP,imp,2},	{CNES_SRE,am_aby,7},{CNES_NOP,am_abx,4,1},{CNES_EOR,am_abx,4,1},{CNES_LSR,am_abx,7},{CNES_SRE,am_abx,7},
	{CNES_RTS,0,6},	{CNES_ADC,am_izx,6},	{CNES_STP,0,0},	{CNES_RRA,am_izx,8},{CNES_NOP,am_zp,3},{CNES_ADC,am_zp,3},{CNES_ROR,am_zp,5},{CNES_RRA,am_zp,5},
	{CNES_PLA,0,4},	{CNES_ADC,am_imm,2},	{CNES_ROR,imp,2},	{CNES_ARR,am_imm,2},{CNES_JMP,am_ind,5},{CNES_ADC,am_abs,4},{CNES_ROR,am_abs,6},{CNES_RRA,am_abs,6},
	{CNES_BVS,am_rel,2,1},{CNES_ADC,am_izy,5,1},{CNES_STP,0,0},	{CNES_RRA,am_izy,8},{CNES_NOP,am_zpx,4},{CNES_ADC,am_zpx,4},{CNES_ROR,am_zpx,6},{CNES_RRA,am_zpx,6},
	{CNES_SEI,imp,2},	{CNES_ADC,am_aby,4,1},{CNES_NOP,imp,2},	{CNES_RRA,am_aby,7},{CNES_NOP,am_abx,4,1},{CNES_ADC,am_abx,4,1},{CNES_ROR,am_abx,7},{CNES_RRA,am_abx,7},
	{CNES_NOP,am_imm,2},	{CNES_STA,am_izx,6},	{CNES_NOP,am_imm,2},	{CNES_SAX,am_izx,6},{CNES_STY,am_zp,3},{CNES_STA,am_zp,3},{CNES_STX,am_zp,3},{CNES_SAX,am_zp,3},
	{CNES_DEY,imp,2},	{CNES_NOP,am_imm,2},	{CNES_TXA,imp,2},	{CNES_XAA,am_imm,2},{CNES_STY,am_abs,4},{CNES_STA,am_abs,4},{CNES_STX,am_abs,4},{CNES_SAX,am_abs,4},
	{CNES_BCC,am_rel,2,1},{CNES_STA,am_izy,6},	{CNES_STP,0,0},	{CNES_AHX,am_izy,6},{CNES_STY,am_zpx,4},{CNES_STA,am_zpx,4},{CNES_STX,am_zpy,4},{CNES_SAX,am_zpy,4},
	{CNES_TYA,imp,2},	{CNES_STA,am_aby,5},	{CNES_TXS,imp,2},	{CNES_TAS,am_aby,5},{CNES_SHY,am_abx,5},{CNES_STA,am_abx,5},{CNES_SHX,am_aby,5},{CNES_AHX,am_aby,5},
	{CNES_LDY,am_imm,2},	{CNES_LDA,am_izx,6},	{CNES_LDX,am_imm,2},	{CNES_LAX,am_izx,6},{CNES_LDY,am_zp,3},{CNES_LDA,am_zp,3},{CNES_LDX,am_zp,3},{CNES_LAX,am_zp,3},
	{CNES_TAY,imp,2},	{CNES_LDA,am_imm,2},	{CNES_TAX,imp,2},	{CNES_LAX,am_imm,2},{CNES_LDY,am_abs,4},{CNES_LDA,am_abs,4},{CNES_LDX,am_abs,4},{CNES_LAX,am_abs,4},
	{CNES_BCS,am_rel,2,1},{CNES_LDA,am_izy,5,1},{CNES_STP,0,0},	{CNES_LAX,am_izy,5,1},{CNES_LDY,am_zpx,4},{CNES_LDA,am_zpx,4},{CNES_LDX,am_zpy,4},{CNES_LAX,am_zpy,4},
	{CNES_CLV,imp,2},	{CNES_LDA,am_aby,4,1},{CNES_TSX,imp,2},	{CNES_LAS,am_aby,4,1},{CNES_LDY,am_abx,4,1},{CNES_LDA,am_abx,4,1},{CNES_LDX,am_aby,4,1},{CNES_LAX,am_aby,4,1},
	{CNES_CPY,am_imm,2},	{CNES_CMP,am_izx,6},	{CNES_NOP,am_imm,2},	{CNES_DCP,am_izx,8},{CNES_CPY,am_zp,3},{CNES_CMP,am_zp,3},{CNES_DEC,am_zp,5},{CNES_DCP,am_zp,5},
	{CNES_INY,imp,2},	{CNES_CMP,am_imm,2},	{CNES_DEX,imp,2},	{CNES_AXS,am_imm,2},{CNES_CPY,am_abs,4},{CNES_CMP,am_abs,4},{CNES_DEC,am_abs,6},{CNES_DCP,am_abs,6},
	{CNES_BNE,am_rel,2,1},{CNES_CMP,am_izy,5,1},{CNES_STP,0,0},	{CNES_DCP,am_izy,8},{CNES_NOP,am_zpx,4},{CNES_CMP,am_zpx,4},{CNES_DEC,am_zpx,6},{CNES_DCP,am_zpx,6},
	{CNES_CLD,imp,2},	{CNES_CMP,am_aby,4,1},{CNES_NOP,imp,2},	{CNES_DCP,am_aby,7},{CNES_NOP,am_abx,4,1},{CNES_CMP,am_abx,4,1},{CNES_DEC,am_abx,7},{CNES_DCP,am_abx,7},
	{CNES_CPX,am_imm,2},	{CNES_SBC,am_izx,6},	{CNES_NOP,am_imm,2},	{CNES_ISC,am_izx,8},{CNES_CPX,am_zp,3},{CNES_SBC,am_zp,3},{CNES_INC,am_zp,5},{CNES_ISC,am_zp,5},
	{CNES_INX,imp,2},	{CNES_SBC,am_imm,2},	{CNES_NOP,imp,2},	{CNES_SBC,am_imm,2},{CNES_CPX,am_abs,4},{CNES_SBC,am_abs,4},{CNES_INC,am_abs,6},{CNES_ISC,am_abs,6},
	{CNES_BEQ,am_rel,2,1},{CNES_SBC,am_izy,5,1},{CNES_STP,0,0},	{CNES_ISC,am_izy,8},{CNES_NOP,am_zpx,4},{CNES_SBC,am_zpx,4},{CNES_INC,am_zpx,6},{CNES_ISC,am_zpx,6},
	{CNES_SED,imp,2},	{CNES_SBC,am_aby,4,1},{CNES_NOP,imp,2},	{CNES_ISC,am_aby,7},{CNES_NOP,am_abx,4,1},{CNES_SBC,am_abx,4,1},{CNES_INC,am_abx,7},{CNES_ISC,am_abx,7},
};


opcode get_opcode(uint8_t op){
    opcode opc;
    opc = instruction_set_tab[op];
    return opc;
}