/* 
 *          UMio.c
 *      Authors: Nick and Max
 *      Due: Monday, November 23
 *      Purpose: The purpose of this module is to hold the functions for the
 *               um which do not have to do with either memory or math, it hold
 *               the IO functions, halt and bitwise NAND.
 */
#include <stdio.h>
#include <stdlib.h>
#include "um-dis.h"
#include "uarray.h"
#include "bitpack.h"
#include "assert.h"
#include "stack.h"
#include "mem.h"

/* Name: halt
 * Parameters: struct containing all aspects of our UM
 * Returns: Nothing
 * Does: Frees all memory used in the UM, then exits with 0
 * Error Cases: None
 */
void halt(Mem used)
{
    /* Freeing memory in all mapped functions */
    for(uint32_t i = 0; i < used->currseg; ++i) {
        if(*(UArray_T *)UArray_at(used->m, i) != NULL) {
            UArray_free((UArray_T *)UArray_at(used->m, i));
        }
    }

    UArray_free(&used->m);
    /* Freeing the stack of recycled segments */
    while(Stack_empty(used->segs) != 1) {
        free((uint32_t *)Stack_pop(used->segs));    
    }

    Stack_free(&used->segs);
    free(used);
    exit(0);
}


/* Name: Output
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: Prints out the value within the inputted register
 * Error Cases: Asserts that the value can be printed and that the register
 *              number is less than 8 (exists)
 */
void output(Mem used, uint32_t c)
{
    assert(used->r[c] < 256);
    assert(c < 8);
    putchar(used->r[c]);
}

/* Name: Input
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: reads characters in and stores them within r[a]
 * Error Cases: If the character is EOF, then it is turned into a
 *              uint32_t of all 1s
 */
void input(Mem used, uint32_t c)
{
    uint32_t a = getchar();
    if(a == (uint32_t) EOF) {
        a = (uint32_t) ~0;
    } else {
        assert(a < 256);
    }
    
    used->r[c] = a;
}


/* Name: nand (Bitwise NAND)
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: Takes the bitwise NAND of the values in registers r[b] and r[c]
 * Error Cases: None
 */
void nand(Mem used, uint32_t a, uint32_t b, uint32_t c)
{
    used->r[a] = ~(used->r[b] & used->r[c]);
}
