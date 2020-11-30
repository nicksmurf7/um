/* 
 *          um.c
 *      Authors: Nick and Max
 *      Due: Monday, November 23
 *      Purpose: Contains main function for UM, which then calls functions
 *               contained in other files. UM.c handles command line inputs
 *               as well as memory allocation and reading in instructions from
 *               a .um file. Utilizes Bitpack.h to unpack 32-bit instructions
 *               as was done in past homeworks. Also utilizes mem.h file that
 *               contains the UM struct that has all aspects of our UM. 
 */
#include <stdio.h>
#include <stdlib.h>
#include "um-dis.h"
#include "uarray.h"
#include "bitpack.h"
#include "assert.h"
#include "stack.h"
#include "UMsegmem.h"
#include "UMmath.h"
#include "UMio.h"


int fileInsts(FILE *fp);
void readInput(UArray_T seg0, FILE *fp);
void runum(Mem state);
void readOP(uint32_t opcode, uint32_t a, uint32_t b, uint32_t c, Mem state);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Error: Invalid Command Line Input");
        exit(EXIT_FAILURE);
    }

    char *input = argv[1];
    FILE *fp = fopen(input, "r");
    int numInstructions = fileInsts(fp);

    /* Getting back to the start of the file */
    fclose(fp);
    fp = fopen(input, "r");

    /* Creating segment 0 and the registers */
    UArray_T segment0 = UArray_new(numInstructions, sizeof(uint32_t)); 
    assert(segment0);
    UArray_T *aptr;
    uint32_t reg[8] = {0};

    /* Initial size for segments was chosen to be numInstructions/4 + 1 */
    int arraysize = (numInstructions / 4) + 1;
    UArray_T n = UArray_new(arraysize, sizeof(aptr));
    assert(n);
    readInput(segment0, fp);
    *(UArray_T *)UArray_at(n, 0) = segment0;
    Mem state;
    state = (Mem) malloc(sizeof(*state));
    assert(state);
    
    /* Setting all values in state to be initial state */
    state->m = n;
    state->r = reg;
    state->counter = 0;
    state->segs = Stack_new();
    state->currseg = 1;

    runum(state);

    return 0;
}


/* Name: fileInsts
 * Parameters: a file pointer fp
 * Returns: The number of instructions a file contains
 * Does: Takes a file pointer and goes to the end, then used that to find
 *         how many bytes are in the, then divided by 4 because there are
 *          4 bytes per instruction
 * Error Cases: If the file pointer is null, file was not properly opened
 */
int fileInsts(FILE *fp)
{
    if (!fp) {
        fprintf(stderr, "Error: Unable to open instructions file \n");
        exit(EXIT_FAILURE);
    }
    
    fseek(fp, 0L, SEEK_END);
    long int len = ftell(fp);
    return len / 4;
}

/* Name: readInput
 * Parameters: UArray for segment 0 and a file pointer 
 * Returns: None
 * Does: Fills in the segment 0 with instructions by getting a character from
 *          the file pointer, adding it to a uint32 then shifting the value
 *          until the full instruction is got
 * Error Cases: If the file pointer is null, file was not properly opened
 */
void readInput(UArray_T seg0, FILE *fp) 
{ 
    if (!fp) {
        fprintf(stderr, "Error: Unable to open instructions file \n");
        exit(EXIT_FAILURE);
    }

    /* Placing all the data in seg0 */
    for (int i = 0; i < UArray_length(seg0); ++i) {
        uint32_t data = 0;
        for (int j = 0; j < 4; j++) {
            data  = data << 8;
            data += fgetc(fp);
        }

       *((uint32_t *) UArray_at(seg0, i)) = data;
    }
    fclose(fp);
}


/* Name: runum
 * Parameters:  Struct containing all aspects of UM
 * Returns: None
 * Does: Runs a um program until halt is called or an error is reached,
 *          unpacks an instruction based on opcode, then reads then calls
 *          readOP for that instruction
 * Error Cases: If the counter is out of bounds of segment 0 exit with
 *              EXIT_FAILURE and output error message
 */
void runum(Mem state)
{
    while(true) {
        uint32_t opcode, a, b, c;
        UArray_T seg0 = (*(UArray_T *)UArray_at(state->m, 0));
        if (state->counter >= (uint32_t)UArray_length(seg0)) {
            fprintf(stderr, "Error: Counter outside of instruction bounds \n");
            exit(EXIT_FAILURE);
        }

        uint32_t word = *((uint32_t *) UArray_at(seg0, state->counter));
        opcode = Bitpack_getu(word, 4, 28);

        /* Checks to see if opcode is load val and unpacks accordingly */
        if (opcode == 13) {
            a = Bitpack_getu(word, 3, 25);
            b = Bitpack_getu(word, 25, 0);
            c = 0;
        } else {
            /* Unpacking all 3 register instructions */
            a = Bitpack_getu(word, 3, 6);
            b = Bitpack_getu(word, 3, 3);
            c = Bitpack_getu(word, 3, 0);
        }
        
        state->counter++;
        readOP(opcode, a, b, c, state);
    }
}

/* Name: readOP
 * Parameters: the uints for OPCODE and a,b,c values in an instruction,
 *              the Mem struct holding all UM data
 * Returns: None
 * Does: Calls a function based on an opcode, with the uint values of 
 *          a,b,c that are necessary for that function
 * Error Cases: If the opcode is not a valid opcode, prints out an error
 *              message and exits with EXIT_FAILURE
 */
void readOP(uint32_t opcode, uint32_t a, uint32_t b, uint32_t c, Mem state)
{
    if(opcode == 0){
        /* Conditional Move */
        condmove(state, a, b, c);
    } else if (opcode == 1) {
        /* Segmented Load */
        segload(state, a, b, c);
    } else if (opcode == 2) {
        /* Segmented Store */
        segstore(state, a, b, c);
    } else if (opcode == 3) {
        /* Addition */
        add(state, a, b, c);
    } else if (opcode == 4) {
        /* Multiplication */
        multiply(state, a, b, c);
    } else if (opcode == 5) {
        /* Division */
        divide(state, a, b, c);
    } else if (opcode == 6) {
        /* Bitwise NAND */
        nand(state, a, b, c);
    } else if (opcode == 7) {
        /* Halt */
        halt(state);
    } else if (opcode == 8) {
        /* Map Segment */
        mapsegment(state, b, c);
    } else if (opcode == 9) {
        /* Unmap Segment */
        unmapsegment(state, c);
    } else if (opcode == 10) {
        /* Output */
        output(state, c);
    } else if (opcode == 11) {
        /* Input */
        input(state, c);
    } else if (opcode == 12) {
        /* Load Program */
        loadprogram(state, b, c);
    } else if (opcode == 13) {
        /* Load Value */
        loadval(state, a, b);
    } else{
        fprintf(stderr, "Error: Not A Valid Opcode \n");
        exit(EXIT_FAILURE);
    }
}