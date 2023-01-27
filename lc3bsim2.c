/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Kushal Vajrala 
    Name 2: Seema Kulkarni
    UTEID 1: kv7287
    UTEID 2: srk2734
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

    int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
    int i;

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
void load_my_program() {
    FILE * prog;
    int ii, word, program_base;
    char* test = "C:/Users/ishac/Documents/Comp Arch Labs/Lab2/xorTest.obj";
    //char* test = "";
    /* Open program file. */
    prog = fopen(test, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", test);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   test, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);

    RUN_BIT = 1;
}

int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
/*    if (argc < 2) {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    } */

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);
    //load_my_program();

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/






/***** Helper Functions for Executing Instructions *************/
enum { BR, ADD, LDB, STB, JSR, AND, LDW, STW, RTI, XOR, xx1, xx2, JMP, SHF, LEA, TRAP };

void setCC(int result) {
    if (result > 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;
    }
    else if (result < 0) {
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }
    else { // result == 0
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
}


int regToValue(int regbit0, int regbit1, int regbit2) {
    return CURRENT_LATCHES.REGS[regbit2*4 + regbit1*2 + regbit0];
}

void storeRegToPC(int regbit0, int regbit1, int regbit2) {
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[regbit2*4 + regbit1*2 + regbit0];
}

void storeValueToPC(int value) {
    NEXT_LATCHES.PC = Low16bits(value);
}

void storeValueToReg(int value, int regbit0, int regbit1, int regbit2) {
    NEXT_LATCHES.REGS[regbit2*4 + regbit1*2 + regbit0] = Low16bits(value);
}

int instructionBinaryToInt(int instr[16], int start_bit, int isSigned) {
    int value = 0;
    for(int i = 0; i <= start_bit; i++) {
        if ((i == start_bit) && isSigned) {
            value += -1 * instr[i] * (1<<i);
        }
        else {
            value += instr[i] * (1<<i);
        }
    }
    return Low16bits(value); // top 16 bits set to 0, for pos & neg nums
}

int loadFromMemory(int address, int byte) {
    int addr = Low16bits(address);
    if (byte) {
        return MEMORY[(addr >> 1)][addr % 2];
    }
    else { // word
        return (MEMORY[(addr >> 1)][1] << 8) + MEMORY[(addr >> 1)][0];
    }
}

void storeToMemory(int address, int source, int byte) {
    int addr = Low16bits(address);
    if (byte) {
        MEMORY[(addr >> 1)][addr % 2] = (source & 0x00FF);
    }
    else { // word
        MEMORY[(addr >> 1)][1] = ((source & 0xFF00) >> 8) & 0x00FF;
        MEMORY[(addr >> 1)][0] = (source & 0x00FF);
    }
}
/***************************************************************/


void process_instruction(){
    /*  function: process_instruction
     *
     *    Process one instruction at a time
     *       -Fetch one instruction (using address listed in PC)
     *       -Decode
     *       -Execute
     *       -Update NEXT_LATCHES + Memory
     */


    /* Fetch Instruction & Increment PC */
    int instruction = (MEMORY[CURRENT_LATCHES.PC>>1][1] << 8) + MEMORY[CURRENT_LATCHES.PC>>1][0];
    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);


    /* Decode Instruction (convert it to binary + calculate opcode) */
    int instr[16] = { 0 };      // holds binary instruction from LSB (at index 0) to MSB (at index 15)
    int index = 0;
    while (instruction != 0) {
        instr[index] = instruction % 2;
        instruction = instruction / 2;
        index++;
    }
    int opcode = instr[15]*8 + instr[14]*4 + instr[13]*2 + instr[12];


    /* Execute Instruction & Update State */
    if (opcode == ADD) {
        int sr1 = regToValue(instr[6], instr[7], instr[8]);
        int arg2;
        if(instr[5] == 0) { // second arg is a register
            arg2 = regToValue(instr[0], instr[1], instr[2]);
        }
        else { // second arg is an immediate
            arg2 = instructionBinaryToInt(instr, 4, 1);
        }

        storeValueToReg((sr1 + arg2), instr[9], instr[10], instr[11]);
    }
    else if (opcode == AND) {
        int sr1 = regToValue(instr[6], instr[7], instr[8]);
        int arg2;
        if(instr[5] == 0) { // second arg is a register
            arg2 = regToValue(instr[0], instr[1], instr[2]);
        }
        else { // second arg is an immediate
            arg2 = instructionBinaryToInt(instr, 4, 1);
        }

        storeValueToReg((sr1 & arg2), instr[9], instr[10], instr[11]);
    }
    else if (opcode == BR) {
        int br_en = (instr[11] & CURRENT_LATCHES.N) + (instr[10] & CURRENT_LATCHES.Z) + (instr[9] & CURRENT_LATCHES.P);
        if (br_en) {
            int offset9 = instructionBinaryToInt(instr, 8, 1);
            storeValueToPC(NEXT_LATCHES.PC + Low16bits(offset9<<1));
        }
    }
    else if (opcode == JMP) {
        storeRegToPC(instr[6], instr[7], instr[8]);
    }
    else if (opcode == JSR) {
        storeValueToReg(NEXT_LATCHES.PC,1,1,1);

        if (instr[11]) { // JSR, uses offset
            int offset11 = instructionBinaryToInt(instr, 10, 1);
            storeValueToPC(NEXT_LATCHES.PC + Low16bits(offset11<<1));
        }
        else { // JSRR, uses register
            storeRegToPC(instr[6], instr[7], instr[8]);
        }
    }
    else if (opcode == LDB) {
        int baseR = regToValue(instr[6], instr[7], instr[8]);
        int boffset6 = instructionBinaryToInt(instr, 5, 1);
        int value = loadFromMemory(baseR + boffset6, 1);
        storeValueToReg(value, instr[9], instr[10], instr[11]);
        setCC(value);
    }
    else if (opcode == LDW) {
        int baseR = regToValue(instr[6], instr[7], instr[8]);
        int offset6 = instructionBinaryToInt(instr, 5, 1);
        int value = loadFromMemory(baseR + (offset6<<1), 0);
        storeValueToReg(value, instr[9], instr[10], instr[11]);
        setCC(value);
    }
    else if (opcode == LEA) {
        int offset9 = instructionBinaryToInt(instr, 8, 1);
        int DRValue = NEXT_LATCHES.PC + Low16bits(offset9<<1);
        storeValueToReg(DRValue, instr[9], instr[10], instr[11]);
    }
    else if (opcode == SHF) {
        int SRValue = regToValue(instr[6], instr[7], instr[8]);
        int shift = instructionBinaryToInt(instr, 3, 0);
        int DRValue = 0;

        if (instr[4] == 0  && instr[5] == 0) { // LSHF
            DRValue = SRValue << shift;
        }
        else if (instr[4] == 1 && instr[5] == 0) { // RSHFL
            DRValue = SRValue >> shift;
        }
        else if (instr[4] == 1 && instr[5] == 1) { // RSHFA
            if (SRValue >= (1<<15)) { // if negative, sign extend negative bit
                DRValue = (SRValue ^ 0xFFFF0000) >> shift;
            }
            else DRValue = SRValue >> shift;
        }
        storeValueToReg(DRValue, instr[9], instr[10], instr[11]);
    }
    else if (opcode == STB) {
        int baseR = regToValue(instr[6], instr[7], instr[8]);
        int sourceR = regToValue(instr[9], instr[10], instr[11]);
        int boffset6 = instructionBinaryToInt(instr, 5, 1);
        storeToMemory(baseR + boffset6, sourceR, 1);
    }
    else if (opcode == STW) {
        int baseR = regToValue(instr[6], instr[7], instr[8]);
        int sourceR = regToValue(instr[9], instr[10], instr[11]);
        int offset6 = instructionBinaryToInt(instr, 5, 1);
        storeToMemory(baseR + (offset6<<1), sourceR, 1);
    }
    else if (opcode == TRAP) {
        int trapvect8 = instructionBinaryToInt(instr, 7, 0);
        storeValueToReg(NEXT_LATCHES.PC,1,1,1);
        storeValueToPC(loadFromMemory((trapvect8 << 1), 0));
    }
    else if (opcode == XOR) {
        int sr1 = regToValue(instr[6], instr[7], instr[8]);
        int arg2;
        if(instr[5] == 0) { // second arg is a register
            arg2 = regToValue(instr[0], instr[1], instr[2]);
        }
        else { // second arg is an immediate
            arg2 = instructionBinaryToInt(instr, 4, 1);
        }

        storeValueToReg((sr1 ^ arg2), instr[9], instr[10], instr[11]);
    }

}