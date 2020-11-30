/* 
 *          mem.h
 *      Authors: Nick and Max
 *      Due: Monday, November 23
 *      Purpose: Contains struct that hold all aspects of our UM, which is
 *               then included by all other files which are run in um. 
 */
#include <stdio.h>
#include <stdlib.h>
#include "um-dis.h"
#include "uarray.h"

#ifndef MEM  
#define MEM

typedef struct Mem *Mem;

/*
 *      m: UArray containing all segments (that are represented as UArrays)
 *      segs: Contains all recycled segments that are used during mapping
 *      r: Contains all 8 32-bit general-purpose registers that are reused
 *      counter: Describes the number of the segment we are currently on  
 *      currseg: Describes the number of the next segment to be mapped
 */
struct Mem {
    UArray_T m;
    Stack_T segs;
    uint32_t *r;
    uint32_t counter; 
    uint32_t currseg;
};

#endif