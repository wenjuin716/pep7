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
void* reg[2]={NULL, NULL};
stat_flags pep7_stat;

void setReg(int index, int16_t value){
  *((int16_t*)reg[index])=value;

  //check the status bit
  if(index == REG_A){
    if(*((int16_t*)reg[REG_A]) == 0)
      pep7_stat.f.regA_z = 1;
    else
      pep7_stat.f.regA_z = 0;

    //if last bit is 1 -> negative
    if(*((int16_t*)reg[REG_A]) & (1<<15) )
      pep7_stat.f.regA_n = 1;
    else
      pep7_stat.f.regA_n = 0;

  }else if(index == REG_X){

    if(*((int16_t*)reg[REG_X]) == 0)
      pep7_stat.f.regX_z = 1;
    else
      pep7_stat.f.regX_z = 0;

    //if last bit is 1 -> negative
    if(*((int16_t*)reg[REG_X]) & (1<<15) )
      pep7_stat.f.regX_n = 1;
    else
      pep7_stat.f.regX_n = 0;
  }
}

void pep7_init(void){

  printf("######## pep/7 init start. ########\n");
  //init pep7 memory
  pep_mem_head = (void*)malloc(MEM_SIZE);
  if(pep_mem_head == NULL){
    printf("pep7 memory init fail\n");
    goto done;
  }

  memset(pep_mem_head, 0, MEM_SIZE);
  //memset(pep_mem_head, (1<<MEM_SIZE)-1, MEM_SIZE);	//all bit is "1"


  //init register
  reg[REG_A] = (void*)malloc(REG_SIZE);
  if(reg[REG_A] == NULL){
    printf("REG_A init fail\n");
    goto done;
  }

  setReg(REG_A, 0);
  //memset(reg[REG_A], 0, REG_SIZE);

  reg[REG_X] = (void*)malloc(REG_SIZE);
  if(reg[REG_X] == NULL){
    printf("REG_X init fail\n");
    goto done;
  }

  setReg(REG_X, 0);
  //memset(reg[REG_X], 0, REG_SIZE);

#if 0	//debug
  printf("MEM_SIZE=%d[%d]\n",MEM_SIZE, ((1<<MEM_SIZE)-1));
  printf("address of pep_mem_head=%p\n",pep_mem_head);
  printf("address of (uint16_t*)pep_mem_head+1=%p\n",(uint16_t*)pep_mem_head+1);
  printf("*pep_mem_head=%d\n",*((uint16_t*)pep_mem_head));
  printf("*(pep_mem_head+1)=%d\n",*((uint16_t*)pep_mem_head+1));
#endif

done:
  printf("######## pep/7 init success. ########\n");
  return;
}

void pep7_shutdown(void){

  printf("######## pep/7 shutdown start. ########\n");
  //free pep7 memory
  if(pep_mem_head != NULL){
    free(pep_mem_head);
  }

  if(reg[REG_A] != NULL){
    free(reg[REG_A]);
  }

  if(reg[REG_X] != NULL){
    free(reg[REG_X]);
  }

  printf("######## pep/7 shutdown success. ########\n");
  return;
}

void dumpInfo(void){
  printf("######## pep/7 dumpInfo ########\n");
  printf("\tRegister A: %d\n", *(int16_t*)reg[REG_A]);
  printf("\tRegister X: %d\n", *(int16_t*)reg[REG_X]);
  printf("\tpep7 stat:\n");
  printf("\t  regX_n: %d\n", pep7_stat.f.regX_n);
  printf("\t  regX_z: %d\n", pep7_stat.f.regX_z);
  printf("\t  regA_n: %d\n", pep7_stat.f.regA_n);
  printf("\t  regA_z: %d\n", pep7_stat.f.regA_z);
  printf("################################\n");
}

void main(int argc, char** argv){
  printf("welcome to pep/7 simulator\n");

  pep7_init();

#if 0
  setReg(REG_A, 1);
  dumpInfo();
  setReg(REG_A, -1);
  dumpInfo();
  setReg(REG_A, 0);
  dumpInfo();
#endif

#if 0	//debug
#if 1
  inst_reg reg;
  //memset(&reg, 0 ,sizeof(reg));
  printf("sizeof(reg)=%d\n", (int)sizeof(reg));
  printf("sizeof(reg.is)=%d\n", (int)sizeof(reg.is));
  printf("sizeof(reg.is.r)=%d\n", (int)sizeof(reg.is.r));
  printf("sizeof(reg.op)=%d\n", (int)sizeof(reg.op));
#endif

#if 1
  stat_flags st_f;

  printf("sizeof(st_f)=%d\n", (int)sizeof(st_f));
#endif 

#if 1
  printf("opcode_type CHAR_OUT=%d\n", CHAR_OUT);
#endif
#endif
  pep7_shutdown();
  return;	
}
