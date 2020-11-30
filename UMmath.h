/* 
 *          UMmath.h
 *      Authors: Nick and Max
 *      Due: Monday, November 23
 *      Purpose: Holds functions for math portion of UM, handles
 *                  add, multiply, divide, loadval and conditional
 *                  move
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"
#include "mem.h"

void add(Mem used, uint32_t a, uint32_t b, uint32_t c);
void multiply(Mem used, uint32_t a, uint32_t b, uint32_t c);
void divide(Mem used, uint32_t a, uint32_t b, uint32_t c);
void loadval(Mem used, uint32_t a, uint32_t value);
void condmove(Mem used, uint32_t a, uint32_t b, uint32_t c);