/* 
 *          UMsegmem.h
 *      Authors: Nick and Max
 *      Due: Monday, November 23
 *      Purpose: Holds functions for functions that
 *               influence segments: mapping and unmapping
 *               segments, segmented load and store and 
 *               load program
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"
#include "mem.h"

void unmapsegment(Mem used, uint32_t c);
void mapsegment(Mem used, uint32_t b, uint32_t c);
void segstore(Mem used, uint32_t a, uint32_t b, uint32_t c);
void segload(Mem used, uint32_t a, uint32_t b, uint32_t c);
void loadprogram(Mem used, uint32_t b, uint32_t c);
