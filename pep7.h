/* 
 *	this file pep7.h is used to define basic information
 *	Pep/7 basic requirement:
 *	 - 32 machine-language instructions
 *	 - Memory:
 *	    The memory unit is made up of 4,096 bytes of storage
 *	 - Registers:
 *	    Pep/7 has seven registers, three of which we focus on at this point
 *		1) The program counter (PC) (2 byte)
 *		2) The instruction register (IR) (3 byte)
 *		3) The accumulator (A register) (2 byte)
 *		4) The index register (X register) (2 byte)
 *	 - Pep/7 has four status bits, of which we examine two:
 *	    N: 1 if the A register is negative, 0 otherwise
 *	    Z: 1 if the A register is zero, 0 otherwise
 */

#include <stdint.h>

/*********** Register  ***********/
#define REG_SIZE 2	// byte
#define REG_A	0	// accumulator register index
#define REG_X	1	// index register index

/*********** Memory ***********/
//#define MEM_SIZE	4096
#define MEM_SIZE	32	// In byte, total 2^6


/*********** Instruction Registers(IR) ***********/
/*
 *	Instruction Specifier:
 *		[operation code(opcode)]
 *			00000: Stop execute
 *			00001: Load operand into register, "reg = operand"
 *			00010: Store the contain of register into operand, "operand = reg"
 *			00011: Add the operand to register,  "reg += operand"
 *			00100: Subtract the operand from register "reg -= operand"
 *			11011: Character input into operand
 *			11100: Character output from operand
 *
 *		[register specifier]
 *			0: register A(the accumulator) is involved in the operation
 *			1: register X(the index register) is involved in the operation
 *
 *		[addressing mode(addr_mode)]
 *			00: Immediate-mode addressing
 *			  In immediate-mode addressing, the operand of register as data.
 *			01: Direct-mode addressing
 *			  In Direct-mode addressing, the operand of register as data pointer which poiter to memory.
 */

#define INST_SPEC_SIZE	1	//byte
#define OPER_SPEC_SIZE	2	//byte
#define INST_REG_SIZE (INST_SPEC_SIZE + OPER_SPEC_SIZE)	//byte

#define IMMEDIATE_MODE	0
#define DIRECT_MODE	1

union ir_inst_spec {
  uint8_t inst_s;

  struct {
    unsigned char addr_mode:2;	//addressing-mode specifier
    unsigned char R:1;		//register specifier
    unsigned char opcode:5;	//operation code
  } r;
};

/*
 *	the operation specifier is depends on "addressing-mode specifier" of instruction specifier
 *	please check the "addressing mode(addr_mode)" description
 */
union ir_op_spec {
  uint16_t inst_operand;
};

typedef struct inst_reg{
  union ir_op_spec op;
  union ir_inst_spec is;
} inst_reg;

/*********** operation code ***********/
/* you can use it to enhance the code readable */
#define REG_SPEC_OFFSET	2
#define OPCODE_OFFSET	3

#define ADDRMODE_MASK	((1<<2)-1)
#define REG_SPEC_MASK	((1<<1)-1)<<REG_SPEC_OFFSET
#define OPCODE_MASK	((1<<5)-1)<<OPCODE_OFFSET


typedef enum opcode_type{
  STOP=0,
  LOAD=1,
  STORE=2,
  ADD=3,
  SUB=4,
  CHAR_IN=27,
  CHAR_OUT=28,	
} opcode_type;

/*********** status bits ***********/
/*	
 *	- Pep/7 has four status bits, of which we examine two:
 *		stat 1 (bit 3)	: unknown
 *		stat 2 (bit 2)	: unknown
 *		N (bit 1)	: 1 if the A register is negative, 0 otherwise
 *		Z (bit 0)	: 1 if the A register is zero, 0 otherwise
 */
typedef union stat_flags {
  uint8_t flags;
  
  struct {
    unsigned char regA_z:1;
    unsigned char regA_n:1;
    unsigned char regX_z:1;	//unknown, tmp define
    unsigned char regX_n:1;	//unknown, tmp define
    unsigned char unused:4;
  } f;
} stat_flags;



