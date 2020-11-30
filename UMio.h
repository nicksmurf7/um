/* 
 *          UMio.h
 *      Authors: Nick and Max
 *      Due: Monday, November 23
 *      Purpose: Contains functions for UMio.c, handles I/O, halt, 
 *               and bitwise NAND, the miscellaneous functions that could
 *               not fit with the other two groups
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"
#include "mem.h"

void output(Mem used, uint32_t c);
void input(Mem used, uint32_t c);
void halt(Mem used);
void nand(Mem used, uint32_t a, uint32_t b, uint32_t c);
