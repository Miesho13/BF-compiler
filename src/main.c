#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include "out_asm.h"
#include "bf_file.h"
#include "stack.h"
#include "rle.h"

int main(int argc, char **argv) {



    if (argc != 2) {
        printf("Error: Invalid argumetn value\n\tUsage: bf [file name]\n");
        return 1;
    }

    bf_file bf = open_file(argv[1]);
    if (bf.len == 0) {
        return 2;
    }
    
    uint8_t *out = NULL;
    size_t out_len = rle(bf.buffer, bf.len, &out);

    asm_out asm_file = crate_asm(out, out_len);

    if (save_out_asm(&asm_file) != 0) {
        return 3;
    }

    if (call_fasm_cpm() != 0) {
        return 3;
    }
    return 0;
}
