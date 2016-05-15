/* 
 *	this file pep7.h is used to define basic information
 *	Pep/7 basic requirement:
 *	 - 32 machine-language instructions
 *	 - Memory:
 *	    The memory unit is made up of 4,096 bytes of storage
 *	 - Registers:
 *	    Pep/7 has seven registers, three of which we focus on at this point
 *		1) The program counter (PC) (contains the address of the next instruction to be executed)
 *		2) The instruction register (IR) (contains a copy of the instruction being executed)
 *		3) The accumulator (A register)
 *		4) The index register (X register), purpose unknown
 *	 - Pep/7 has four status bits, of which we examine two:
 *	    N: 1 if the A register is negative, 0 otherwise
 *	    Z: 1 if the A register is zero, 0 otherwise
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pep7.h"

void* pep_mem_head=NULL;	//the memory head of pep/7's memory, and memory size should be "MEM_SIZE"
/*
 *	reg[0]: accumulator register 
 *	reg[1]: index register 
 */
int16_t* pep7_reg[2]={NULL, NULL};
stat_flags pep7_stat;

inst_reg* pep7_ir=NULL;


void setReg(int index, int16_t value){
  *((int16_t*)pep7_reg[index])=value;

  //check the status bit
  if(index == REG_A){
    if(*(pep7_reg[REG_A]) == 0)
      pep7_stat.f.regA_z = 1;	//reg A is zero
    else
      pep7_stat.f.regA_z = 0;

    //if last bit is 1 -> negative
    if(*(pep7_reg[REG_A]) & (1<<15) )
      pep7_stat.f.regA_n = 1;	//reg A is negative
    else
      pep7_stat.f.regA_n = 0;

  }else if(index == REG_X){

    if(*(pep7_reg[REG_X]) == 0)
      pep7_stat.f.regX_z = 1;	//reg X is zero
    else
      pep7_stat.f.regX_z = 0;

    //if last bit is 1 -> negative
    if(*(pep7_reg[REG_X]) & (1<<15) )
      pep7_stat.f.regX_n = 1;	//reg X is negative
    else
      pep7_stat.f.regX_n = 0;
  }

  return;
}

void pep7_init(void){

  printf("\n######## pep/7 init start. ########\n");
  //init pep7 memory
  pep_mem_head = (void*)malloc(MEM_SIZE);
  if(pep_mem_head == NULL){
    printf("pep7 memory init fail\n");
    goto done;
  }

#if 0
  memset(pep_mem_head, 0, MEM_SIZE);
  memset(pep_mem_head, 0xFF, MEM_SIZE);	//all bit is "1"
#else
  int i=0;
  for(i=0; i<MEM_SIZE; i++){
      memset(pep_mem_head+i, i+65, 1);
  }
  //hexdump(pep_mem_head, MEM_SIZE);
#endif
#if DEBUG
  hexdump(pep_mem_head, MEM_SIZE);
#endif

  //init pep7 register
  pep7_reg[REG_A] = (void*)malloc(REG_SIZE);
  if(pep7_reg[REG_A] == NULL){
    printf("REG_A init fail\n");
    goto done;
  }
  setReg(REG_A, 0);

  pep7_reg[REG_X] = (void*)malloc(REG_SIZE);
  if(pep7_reg[REG_X] == NULL){
    printf("REG_X init fail\n");
    goto done;
  }
  setReg(REG_X, 0);

#if DEBUG
  dumpRegInfo();
#endif

  //init instruction register
  pep7_ir = (inst_reg*)malloc(INST_REG_SIZE);
  if(pep7_ir == NULL){
    printf("pep7_ir init fail\n");
    goto done;
  }
  memset(pep7_ir, 0, INST_REG_SIZE);

#if DEBUG
  dumpIntsRegInfo(pep7_ir);
#endif
done:
  printf("######## pep/7 init success. ########\n");
  return;
}

void pep7_shutdown(void){

  printf("\n######## pep/7 shutdown start. ########\n");
  //STEP1:free pep7 memory
  if(pep_mem_head != NULL){
    free(pep_mem_head);
  }

  //STEP2:free pep7 registers
  if(pep7_reg[REG_A] != NULL){
    free(pep7_reg[REG_A]);
  }

  if(pep7_reg[REG_X] != NULL){
    free(pep7_reg[REG_X]);
  }

  //STEP3:free instruction register 
  if(pep7_ir != NULL){
    free(pep7_ir);
  }

  printf("######## pep/7 shutdown success. ########\n");
  return;
}

void dumpRegInfo(void){
  printf("\n######## pep/7 dumpRegInfo ########\n");
  printf("\tRegister A: %d\n", *pep7_reg[REG_A]);
  printf("\tRegister X: %d\n", *pep7_reg[REG_X]);
  printf("\tpep7 stat:\n");
  printf("\t  regX_n: %d\n", pep7_stat.f.regX_n);
  printf("\t  regX_z: %d\n", pep7_stat.f.regX_z);
  printf("\t  regA_n: %d\n", pep7_stat.f.regA_n);
  printf("\t  regA_z: %d\n", pep7_stat.f.regA_z);
  printf("################################\n");
  return;
}

void dumpIntsRegInfo(inst_reg* ir){
  printf("\n######## Instructuin Reg ########\n");

  if(ir == NULL)
    return;

  printf("Oper Specifier:");
  hexdump(&(ir->op), OPER_SPEC_SIZE);
  printf("Inst Specifier:");
  hexdump(&(ir->is), INST_SPEC_SIZE);

#if 1
  printf("\toperation code=%d\n", ((ir->is).r.opcode));
  printf("\tregister specifier=%d\n", ((ir->is).r.R));
  printf("\taddressing mode=%d\n", ((ir->is).r.addr_mode));
#else
  printf("\toperation code=%d\n", (((ir->is).inst_s)&OPCODE_MASK)>>OPCODE_OFFSET);
  printf("\tregister specifier=%d\n", (((ir->is).inst_s)&REG_SPEC_MASK)>>REG_SPEC_OFFSET);
  printf("\taddressing mode=%d\n", (((ir->is).inst_s)&ADDRMODE_MASK));
#endif

  //check the addressing mode to dump the data.
  printf("Data:");
  if(((ir->is).r.addr_mode) == 0)
    hexdump(&(ir->op), 1);
  else if(((ir->is).r.addr_mode) == 1)
    hexdump(pep_mem_head+(ir->op.inst_operand), 1);

  printf("################################\n");
  return;
}

void excuteInst(inst_reg* ir){
  if(ir == NULL)
    return;

  switch((ir->is).r.opcode){
    case STOP:
      printf("excute STOP instruction.\n");
      break;
    case LOAD:
      printf("excute LOAD instruction.\n");
      break;
    case STORE:
      printf("excute STORE instruction.\n");
      break;
    case ADD:
      printf("excute ADD instruction.\n");
      break;
    case SUB:
      printf("excute SUB instruction.\n");
      break;
    case CHAR_IN:
      printf("excute CHAR_IN instruction.\n");
      break;
    case CHAR_OUT:
      printf("excute CHAR_OUT instruction.\n");
      if(((ir->is).r.addr_mode) == 0)
        putchar(0xFF & ((char)(ir->op.inst_operand)));
      else if(((ir->is).r.addr_mode) == 1){
        putchar(0xFF & *(char*)(pep_mem_head+(ir->op.inst_operand)));
        //hexdump(pep_mem_head+(ir->op.inst_operand), 1);
      }

      break;
    default:
      printf("ERROR:unknown instruction.\n");
      break;
  }

  return;
}

void main(int argc, char** argv){
  printf("welcome to pep/7 simulator\n");

  pep7_init();

#if 1	//instruction register debug
  //int tmp=0xE00048;	//pass operand to stdout
  int tmp=0xE10005;	//pass 5th pep7 memory byte(*operand) to stdout
  //inst_reg* x = malloc(INST_REG_SIZE);
  
  memcpy(pep7_ir, &tmp, INST_REG_SIZE);
  //dumpIntsRegInfo(pep7_ir);

  excuteInst(pep7_ir);

  //free(x);
#endif

#if 0	//pep7 register debug
  setReg(REG_X, -10);
  dumpRegInfo();
  setReg(REG_A, 12);
  dumpRegInfo();
  setReg(REG_A, 0);
  dumpRegInfo();

  setReg(REG_X, 123);
  dumpRegInfo();
  setReg(REG_A, -44);
  dumpRegInfo();
  setReg(REG_X, 0);
  dumpRegInfo();
#endif

  pep7_shutdown();
  return;	
}
