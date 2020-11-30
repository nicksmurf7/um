/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
    Um_instruction output = 0;
    output = Bitpack_newu(output, 3, 0, rc);
    output = Bitpack_newu(output, 3, 3, rb);
    output = Bitpack_newu(output, 3, 6, ra);
    output = Bitpack_newu(output, 4, 28, op);
    return output;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
    Um_instruction output = 0;
    output = Bitpack_newu(output, 25, 0, val);
    output = Bitpack_newu(output, 3, 25, ra);
    output = Bitpack_newu(output, 4, 28, 13);
    return output;
}


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

Um_instruction condmove(Um_register a, Um_register b, Um_register c) 
{
        return three_register(CMOV, a, b, c);
}

Um_instruction nand(Um_register a, Um_register b, Um_register c)
{
        return three_register(NAND, a, b, c);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

Um_instruction input(Um_register c)
{
        return three_register(IN, 0, 0, c);
}

Um_instruction mapsegment(Um_register b, Um_register c)
{
        return three_register(ACTIVATE, 0, b, c);
}

Um_instruction unmapsegment(Um_register c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

Um_instruction segload(Um_register a, Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

Um_instruction segstore(Um_register a, Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

Um_instruction divide(Um_register a, Um_register b, Um_register c)
{
        return three_register(DIV, a, b, c);
}

Um_instruction multiply(Um_register a, Um_register b, Um_register c)
{
        return three_register(MUL, a, b, c);
}

Um_instruction loadprogram(Um_register b, Um_register c)
{
        return three_register(LOADP, 0, b, c);
}
/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_add_test(Seq_T stream)
{
        append(stream, loadval(r5, 25));
        append(stream, loadval(r4, 24));
        append(stream, add(r6, r4, r5));
        append(stream, output(r6));
        append(stream, loadval(r5, 4194304));
        append(stream, loadval(r4, 4194304));
        append(stream, add(r6, r4, r5));
        append(stream, add(r6, r6, r6));
        append(stream, add(r6, r6, r6));
        append(stream, add(r6, r6, r6));
        append(stream, add(r6, r6, r6));
        append(stream, add(r6, r6, r6));
        append(stream, add(r6, r6, r6));
        append(stream, add(r6, r6, r6));
        append(stream, add(r6, r6, r6));
        append(stream, add(r6, r6, r6));
        append(stream, loadval(r4, 56));
        append(stream, add(r6, r4, r6));
        append(stream, output(r6));
        append(stream, halt());
}

void build_mult_test(Seq_T stream)
{
        append(stream, loadval(r1, 2));
        append(stream, loadval(r2, 25));
        append(stream, multiply(r3, r1, r2));
        append(stream, output(r3));
        append(stream, loadval(r4, 833327));
        append(stream, loadval(r5, 5154));;
        append(stream, multiply(r3, r4, r5));
        append(stream, output(r3));
        append(stream, halt());
}

void build_div_test(Seq_T stream)
{
        append(stream, loadval(r5, 2));
        append(stream, loadval(r4, 'd'));
        append(stream, divide(r6, r4, r5));
        append(stream, output(r6));
        append(stream, halt());
}

void build_IO_test(Seq_T stream)
{
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, halt());
}


void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}


void build_printsix_test(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}


void build_mapunmap_test(Seq_T stream)
{
        append(stream, loadval(r1, 5));
        append(stream, mapsegment(r2, r1));
        append(stream, unmapsegment(r2));
        append(stream, mapsegment(r2, r1));
        append(stream, mapsegment(r3, r1));
        append(stream, unmapsegment(r3));
        append(stream, unmapsegment(r2));
        append(stream, mapsegment(r2, r1));
        append(stream, mapsegment(r3, r1));
        append(stream, mapsegment(r4, r1));
        append(stream, halt());
}

void build_segload_test(Seq_T stream)
{
        append(stream, loadval(r1, 5));
        append(stream, loadval(r2, 1));
        append(stream, mapsegment(r2, r1));
        append(stream, mapsegment(r5, r1));
        append(stream, mapsegment(r6, r1));
        append(stream, loadval(r3, 3));
        append(stream, loadval(r4, 54));
        append(stream, segstore(r2, r3, r4));
        append(stream, loadval(r4, 58));
        append(stream, segstore(r5, r3, r4));
        append(stream, loadval(r4, 62));
        append(stream, segstore(r6, r3, r4));
        append(stream, segload(r1, r2, r3));
        append(stream, segload(r4, r5, r3));
        append(stream, segload(r6, r6, r3));
        append(stream, output(r6));
        append(stream, output(r4));
        append(stream, output(r1));
        append(stream, unmapsegment(r2));
        append(stream, halt());
}

void build_condmove_test(Seq_T stream)
{
        append(stream, loadval(r1, 56));
        append(stream, loadval(r2, 72));
        append(stream, loadval(r3, 1));
        append(stream, loadval(r4, 0));
        append(stream, condmove(r1, r2, r4));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, condmove(r1, r2, r3));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, halt());
}

void build_nand_test(Seq_T stream)
{
        append(stream, loadval(r1, 30174645));
        append(stream, loadval(r2, 3379786));
        append(stream, loadval(r3, 0));
        append(stream, loadval(r4, 56));
        append(stream, nand(r3, r1, r2));
        append(stream, add(r3, r3, r4));
        append(stream, output(r3));
        append(stream, halt());
}

void build_loadprogram_test(Seq_T stream)
{
        append(stream, loadval(r1, 6));
        append(stream, loadval(r3, 2));
        append(stream, mapsegment(r2, r1));
        append(stream, loadval(r4, 57344));
        append(stream, loadval(r5, 32768));
        append(stream, multiply(r6, r4, r5));
        append(stream, segstore(r2, r3, r6));
        append(stream, loadval(r3, 0));
        append(stream, loadval(r1, 7444));
        append(stream, loadval(r0, 473296));
        append(stream, multiply(r1, r1, r0));
        append(stream, segstore(r2, r3, r1));
        append(stream, loadval(r3, 1));
        append(stream, loadval(r1, 21287));
        append(stream, loadval(r0, 126103));
        append(stream, multiply(r1, r1, r0));
        append(stream, segstore(r2, r3, r1));
        append(stream, loadval(r7, 0));
        append(stream, loadprogram(r2, r7));
}

void build_50mil_test(Seq_T stream)
{
    append(stream, loadval(r5, 0));    
    append(stream, loadval(r7, 6250000));
    append(stream, loadval(r2, 1));
    append(stream, add(r1, r1, r2));
    append(stream, divide(r6, r1, r7));
    append(stream, loadval(r4, 8));
    append(stream, condmove(r5, r4, r6));
    append(stream, loadprogram(r3, r5));
    append(stream, halt());
}