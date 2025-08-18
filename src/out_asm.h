#ifndef __OUT_ASM__
#define __OUT_ASM__

#include <stdint.h>
#include <stdlib.h>
#include "bf_file.h"

typedef struct {
    uint8_t *buffer;
    size_t count;
} asm_out;

typedef enum  { 
    INC = '>',
    DEC = '<',
    ADD = '+',
    SUB = '-',
    OUT = '.',
    GET = ',',
    LABEL = '[',
    JMP = ']',
} TOKENS;

int add_asm_line(asm_out *out, const char* line);
asm_out crate_asm(uint8_t *rle_bf, size_t rle_bf_len);
int call_fasm_cpm();
int save_out_asm(asm_out *out);

#endif
