/* 
 *          UMmath.c
 *      Authors: Nick and Max
 *      Due: Monday, November 23
 *      Purpose: Handles all arithmetic instructions to UM,
 *                  add, multiply, divide, loadval and 
 *                  conditional move
 */
#include <stdio.h>
#include <stdlib.h>
#include "um-dis.h"
#include "uarray.h"
#include "readaline.h"
#include "bitpack.h"
#include "assert.h"
#include "stack.h"
#include "mem.h"


/* Name: Add
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: Adds the values within r[b] and r[c], mods by 2^32 and then
 *       stores that value within r[a]
 * Error Cases: None
 */
void add(Mem used, uint32_t a, uint32_t b, uint32_t c)
{
    used->r[a] = ((used->r[b] + used->r[c]) % 4294967296);
}


/* Name: Multiply
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: Multiplies the values within r[b] and r[c], mods by 2^32 and then
 *       stores that value within r[a]
 * Error Cases: None
 */
void multiply(Mem used, uint32_t a, uint32_t b, uint32_t c)
{
    used->r[a] = ((used->r[b] * used->r[c]) % 4294967296);
}


/* Name: Divide
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: Divides the values within r[b] and r[c] and then
 *       stores that value within r[a]
 * Error Cases: if the value within r[c] is 0, then an error is printed
 *              and UM exits with code EXIT_FAILURE
 */
void divide(Mem used, uint32_t a, uint32_t b, uint32_t c)
{
    if(used->r[c] == 0) {
        fprintf(stderr, "Error: Divide by 0");
        exit(EXIT_FAILURE);
    }
    
    used->r[a] = (used->r[b] / used->r[c]);
}


/* Name: loadval
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: Loads value into register r[a]
 * Error Cases: Assert that the register number is less than 8
 */
void loadval(Mem used, uint32_t a, uint32_t value)
{
    assert(a < 8);
    used->r[a] = value;
}


/* Name: condmove (Conditional Move)
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: If the value within r[c] is not zero, the value witthin r[b]
 *       is stored in r[a]
 * Error Cases: None
 */
void condmove(Mem used, uint32_t a, uint32_t b, uint32_t c)
{
    if(used->r[c] != 0) {
        used->r[a] = used->r[b];
    }
}
