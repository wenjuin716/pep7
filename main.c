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

#define PEP7_DEFAULT_FILE "PEP7_default"

void* pep_mem_head=NULL;	//the memory head of pep/7's memory, and memory size should be "MEM_SIZE"
/*
 *	reg[0]: accumulator register 
 *	reg[1]: index register 
 */
int16_t* pep7_reg[2]={NULL, NULL};
stat_flags pep7_stat;

uint16_t *progCounter=NULL;	//program counter, pointer to text segment of pep7 memory
inst_reg* pep7_ir=NULL;		//current instruction register

extern char *optarg;

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

#if 1
  memset(pep_mem_head, 0, MEM_SIZE);
  //memset(pep_mem_head, 0xFF, MEM_SIZE);	//all bit is "1"
#else
  int i=0;
  for(i=0; i<MEM_SIZE; i++){
      memset(pep_mem_head+i, i, 1);	//0~
//      memset(pep_mem_head+i, i+65, 1);	//A-Z
  }
  hexdump(pep_mem_head, MEM_SIZE);
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
  printf("\tRegister A: %d\t(0x%x)\n", *pep7_reg[REG_A],*pep7_reg[REG_A]);
  printf("\tRegister X: %d\t(0x%x)\n", *pep7_reg[REG_X],*pep7_reg[REG_X]);
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
  if(((ir->is).r.addr_mode) == IMMEDIATE_MODE)
    hexdump(&(ir->op), OPER_SPEC_SIZE);
  else if(((ir->is).r.addr_mode) == DIRECT_MODE)
    hexdump(pep_mem_head+(ir->op.inst_operand), OPER_SPEC_SIZE);

  printf("################################\n");
  return;
}

void* getPep7Mem(uint16_t offset){
  return (pep_mem_head+offset);
}

void excuteInst(inst_reg* ir){
  if(ir == NULL)
    return;

  switch((ir->is).r.opcode){
    case STOP:
      //printf("excute STOP instruction.\n");

      hexdump(pep_mem_head, MEM_SIZE);
      dumpRegInfo();
      dumpIntsRegInfo(ir);
      break;
    case LOAD:
      //printf("excute LOAD instruction.\n");
      /* Load data to REG_A */
      if(((ir->is).r.addr_mode) == IMMEDIATE_MODE){
        setReg(REG_A, (uint16_t)(ir->op.inst_operand));
      }else if(((ir->is).r.addr_mode) == DIRECT_MODE){
        setReg(REG_A, *(uint16_t*)getPep7Mem(ir->op.inst_operand));
      }
      break;
    case STORE:
      //printf("excute STORE instruction.\n");
      /* STORE REG_A data to operand */
      if(((ir->is).r.addr_mode) == IMMEDIATE_MODE){
        memcpy(&(ir->op.inst_operand), pep7_reg[REG_A], REG_SIZE);
      }else if(((ir->is).r.addr_mode) == DIRECT_MODE){
        memcpy((uint16_t*)getPep7Mem(ir->op.inst_operand), pep7_reg[REG_A], REG_SIZE);
      }
      break;
    case ADD:
      //printf("excute ADD instruction.\n");
      /* ADD operand Data to register A */
      if(((ir->is).r.addr_mode) == IMMEDIATE_MODE){
        setReg(REG_A, (*pep7_reg[REG_A]+(uint16_t)(ir->op.inst_operand)));
      }else if(((ir->is).r.addr_mode) == DIRECT_MODE){
        setReg(REG_A, (*pep7_reg[REG_A]+*(uint16_t*)getPep7Mem(ir->op.inst_operand)));
      }
      break;
    case SUB:
      //printf("excute SUB instruction.\n");
      /* SUB operand Data to register A */
      if(((ir->is).r.addr_mode) == IMMEDIATE_MODE){
        setReg(REG_A, (*pep7_reg[REG_A]-(uint16_t)(ir->op.inst_operand)));
      }else if(((ir->is).r.addr_mode) == DIRECT_MODE){
        setReg(REG_A, (*pep7_reg[REG_A]-*(uint16_t*)getPep7Mem(ir->op.inst_operand)));
      }
      break;
    case CHAR_IN:
      //printf("excute CHAR_IN instruction.\n");
      if(((ir->is).r.addr_mode) != DIRECT_MODE){
        printf("Error: wrong addressing mode[%d], it must be DIRECT_MODE.\n", ((ir->is).r.addr_mode));
        break;
      }

      (*(char*)getPep7Mem(ir->op.inst_operand)) = getchar();
      break;
    case CHAR_OUT:
      //printf("excute CHAR_OUT instruction.\n");
      if(((ir->is).r.addr_mode) == IMMEDIATE_MODE)
        putchar(0xFF & ((char)(ir->op.inst_operand)));
      else if(((ir->is).r.addr_mode) == DIRECT_MODE){
        putchar(0xFF & *(char*)getPep7Mem(ir->op.inst_operand));
      }
      break;
    default:
      printf("ERROR:unknown instruction.\n");
      break;
  }

  return;
}

void usage(char *progName)
{
   printf("usage: %s [-h] [-f FileName] [-s]\n", progName);
   printf("       h: dump the pep7 usage\n");
   printf("       f: input instruction file\n");
   printf("       s: run the default \"Hello\" case\n");
}

void main(int argc, char** argv){
  FILE* prog_fp=NULL;	//use to read instruction file
  char* pep7_file=NULL;
  uint8_t flag = 1;

#if 0
  char buffer[INST_REG_SIZE*2]={0};
#else
  char buff_is[2]={0};
  char buff_ip[4]={0};
#endif
  int32_t c;

  while ((c = getopt(argc, argv, "hf:s")) != -1){
    switch(c)
    {
      case 'h':
        usage(argv[0]);
        break;
      case 'f':
        pep7_file = malloc(strlen(optarg));
        strncpy(pep7_file, optarg, strlen(optarg));
        break;
      case 's':
        pep7_file = malloc(strlen(PEP7_DEFAULT_FILE));
        strncpy(pep7_file, PEP7_DEFAULT_FILE, strlen(PEP7_DEFAULT_FILE));
        break;
      default:
        usage(argv[0]);
        break;
    }
  }

  if(pep7_file == NULL){
    printf("No instrucntion file, stop pep7.\n");
    usage(argv[0]);
    return;
  }  

  if((prog_fp=fopen(pep7_file, "r")) == NULL){
    printf("ERROR: open \"%s\" fail.\n", pep7_file);
    goto finish;
  }

  printf("welcome to pep/7 simulator\n");
  pep7_init();

  while(flag){
#if 0
/*	pep7 main loop, open the instruction file and analysis it until the "00ZZ"
**	file should be hex string format:
**	E00048E00065E0006CE0006CE0006F00ZZ
*/
    memset(buffer, 0 , sizeof(buffer));
    fread( buffer, INST_SPEC_SIZE*2, 1, prog_fp);

    /*	
    **	if it is NOT equal "00", it should be a vailid insturction register
    **  then get the operation specifier
     */
    if(!strncmp(buffer, "00" , INST_SPEC_SIZE*2)){
      //printf("buffer = %s\n", buffer);
      flag = 0;
      continue;
    }

    //instruction specifier 
    pep7_ir->is.inst_s = strtol(buffer, NULL, 16);

    //get instruction operation specifer
    memset(buffer, 0 , sizeof(buffer));
    fread( buffer, OPER_SPEC_SIZE*2, 1, prog_fp);
    pep7_ir->op.inst_operand = strtol(buffer, NULL, 16);

    //dumpIntsRegInfo(pep7_ir);
    excuteInst(pep7_ir);
#else
/*	pep7 main loop, open the instruction file and analysis it until the "00ZZ"
**	the file is hex string file, and each instruction register should be this format: is(1 byte) op(2 byte)
**	such as:
**	E0 00 48 E0 00 65 E0 00 6C E0 00 6C E0 00 6F 00 ZZ
**	^^ ^^^^^
**	is operantion
*/
    memset(buff_is, 0 , sizeof(buff_is));
    fscanf(prog_fp, "%s", buff_is);
    //printf("buff_is = %s\n", buff_is);

    /*	
    **	if it is NOT equal "00", it should be a vailid insturction register
    **  then get the operation specifier
     */
    if(!strncmp(buff_is, "00" , INST_SPEC_SIZE*2)){
      //printf("buffer = %s\n", buffer);
      flag = 0;
      continue;
    }

    //instruction specifier 
    pep7_ir->is.inst_s = strtol(buff_is, NULL, 16);

    memset(buff_ip, 0 , sizeof(buff_ip));
    fscanf(prog_fp, "%s", buff_ip);
    fscanf(prog_fp, "%s", &buff_ip[2]);
    //printf("buff_ip = %s\n", buff_ip);

    pep7_ir->op.inst_operand = strtol(buff_ip, NULL, 16);
    excuteInst(pep7_ir);
#endif
  }

finish:
  if(pep7_file)
    free(pep7_file);

  if(prog_fp != NULL)
    fclose(prog_fp);

  pep7_shutdown();
  return;	
}
