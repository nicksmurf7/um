/* 
 *          UMsegmem.c
 *      Authors: Nick and Max
 *      Due: Monday, November 23
 *      Purpose: Holds functions for functions that
 *               influence segments: mapping and unmapping
 *               segments, segmented load and store and 
 *               load program.
 */
#include <stdio.h>
#include <stdlib.h>
#include "um-dis.h"
#include "uarray.h"
#include "bitpack.h"
#include "assert.h"
#include "stack.h"
#include "mem.h"



/* Name: mapsegment (Map Segment)
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: A new segment is created with a number of
 *       words equal to the value in r[C]. Each word in
 *       the new segment is initialized to 0. A bit pattern
 *       that is not all zeroes and that does not identify
 *       any currently mapped segment is placed in r[B].
 *       The new segment is mapped as m[r[B]].
 * Error Cases: If the size of the newly mapped segment is greater than
 *              the greatest possible size, then an error is printed and
 *              the UM exits with code EXIT_FAILURE, asserts
 *              that UArrays are actually created
 */
void mapsegment(Mem used, uint32_t b, uint32_t c)
{
    uint32_t index = 0;
    /* Checks to see if the stack of used segments is empty */
    if(Stack_empty(used->segs) == 1) {
        if (used->currseg >= (uint32_t)UArray_length(used->m)) {
            /* Making sure the segment size isn't too big */
            uint32_t newsize = used->currseg * 2 + 1;
            if (used->currseg >= 4294967290/2) {
                newsize = 4294967295;
            }

            if (used->currseg > newsize) {
                fprintf(stderr, "Error: New size is less than curr index \n");
                exit(EXIT_FAILURE);
            }

            /* Resizes the UArray of segments to the new size */
            UArray_resize(used->m, newsize);
        }   

        index = used->currseg; 
        used->currseg++;
    } else {
        /* Gets a value from the stack if its not empty */
        uint32_t *uip = ((uint32_t *) Stack_pop(used->segs));
        index = *uip;
        free(uip);
    }

    UArray_T newseg = UArray_new(used->r[c], sizeof(uint32_t));
    assert(newseg);
    *(UArray_T *)UArray_at(used->m, index) = newseg;
    used->r[b] = index;
}

/* Name: unmapsegment (Unmap Segment)
 * Parameters: struct containing all aspects of UM, register to be used
 * Returns: Nothing
 * Does: Segment r[c] is unmapped
 * Error Cases: If r[c] is NULL or if r[c] is segment 0, then an
 *              error is printed and the UM exits with code 
 *              EXIT_FAILURE
 */
void unmapsegment(Mem used, uint32_t c)
{
    if (*(UArray_T *)UArray_at(used->m, used->r[c]) == NULL) {
        fprintf(stderr, "Error: Unmapping NULL segment \n");
        exit(EXIT_FAILURE);
    }

    if (used->r[c] == 0) {
        fprintf(stderr, "Error: Unmapping Segment 0 \n");
        exit(EXIT_FAILURE);
    }

    UArray_free((UArray_T *)UArray_at(used->m, used->r[c]));
    /* allocating new space for uint in the stack */
    uint32_t *segindex = (uint32_t *)malloc(sizeof(uint32_t));
    assert(segindex);
    *segindex = used->r[c];
    Stack_push(used->segs, segindex);
}


/* Name: segstore (Segmented Store)
 * Parameters: struct containing all aspects of our UM, registers to be used
 * Returns: Nothing
 * Does: segment m[r[a]r[b]] gets the value in r[c]
 * Error Cases: If the instruction is for an unmapped segment to be
 *              stored or  if the value inside r[b] is outside
 *              the bounds of our UM  then an error is printed
 *              and the UM exits with code EXIT_FAILURE
 */
void segstore(Mem used, uint32_t a, uint32_t b, uint32_t c)
{
    if (*(UArray_T *)UArray_at(used->m, used->r[a]) == NULL) {
         fprintf(stderr, "Error: Storing into unmapped segment \n");
         exit(EXIT_FAILURE);
     } 

    if (used->r[b] >= (uint32_t)UArray_length(
                    *(UArray_T *)UArray_at(used->m, used->r[a]))) {
        fprintf(stderr, "Error: Storing into out of bounds offset \n");
        exit(EXIT_FAILURE);
    }

    UArray_T segment = *(UArray_T *)UArray_at(used->m, used->r[a]);
    *(uint32_t *)UArray_at(segment, used->r[b]) = used->r[c];
}

/* Name: segload (Segmented Load)
 * Parameters: struct containing all aspects of UM, registers to be used
 * Returns: Nothing
 * Does: r[a] gets the value stored in segment m[r[b]r[c]]
 * Error Cases: If the instruction is for an unmapped segment to be 
 *              loaded or if the value inside r[c] is outside 
 *              of the bounds of our UM, an error is printed and 
 *              the UM exits with code EXIT_FAILURE
 */
void segload(Mem used, uint32_t a, uint32_t b, uint32_t c)
{
    if (*(UArray_T *)UArray_at(used->m, used->r[b]) == NULL) {
         fprintf(stderr, "Error: Loading into unmapped segment \n");
         exit(EXIT_FAILURE);
    } 

    if (used->r[c] >= (uint32_t)UArray_length(
                    *(UArray_T *)UArray_at(used->m, used->r[b]))) {
        fprintf(stderr, "Error: Loading into out of bounds offset \n");
        exit(EXIT_FAILURE);
    }

    UArray_T segment = *(UArray_T *)UArray_at(used->m, used->r[b]);
    used->r[a]       = *(uint32_t *)UArray_at(segment, used->r[c]);
}

/* Name: loadprogram
 * Parameters: Struct containing all aspects of UM, registers to be used
 * Returns: Nothing
 * Does: Segment m[r[b]] is duplicated and that duplicate replaces m[0]
 *          aka segment 0. The program counter is set to r[c]
 * Error Cases: If the instruction is for an unmapped segment to be 
 *              duplicated or if the value inside r[c] is outside 
 *              of the bounds of our UM, an error is printed and 
 *              the UM exits with code EXIT_FAILURE
 */
void loadprogram(Mem used, uint32_t b, uint32_t c)
{
    if (*(UArray_T *)UArray_at(used->m, used->r[b]) == NULL) {
         fprintf(stderr, "Error: Loading program from unmapped segment \n");
         exit(EXIT_FAILURE);
    } 

    if (used->r[c] >= (uint32_t)UArray_length(
                    *(UArray_T *)UArray_at(used->m, used->r[b]))) {
        fprintf(stderr, "Error: Loading counter from out of bounds offset \n");
        exit(EXIT_FAILURE);
    }

    if(used->r[b] == 0) {
        /* If segment is 0, only need to change the counter */
        used->counter = used->r[c];
    } else {
        /* Frees the segment at 0 */
        UArray_free((UArray_T *)UArray_at(used->m, 0));

        /* copies and sets the segment */
        UArray_T segment = *(UArray_T *)UArray_at(used->m, used->r[b]);
        UArray_T temp    = UArray_copy(segment, UArray_length(segment));
        (*(UArray_T *)UArray_at(used->m, 0)) = temp;
        used->counter = used->r[c];
    }
}   