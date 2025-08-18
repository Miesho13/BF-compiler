#include <stdint.h>
#include <assert.h>
#include "rle.h"



//  count      char
//  u8[0-255] u8[0-255]
//  u8[0-255] u8[0-255]
//  u8[0-255] u8[0-255]



// lgg
//
// lllllllllllllllllllgg
//                    ^
// output <- ([0-255] [l])
//
void memory_resize_if_needed(uint8_t *output, size_t output_len, uint32_t input_size) {
    if (output_len < input_size) {
        return; 
    }
    output = realloc(output, input_size*2);
}

void encode_block(uint8_t *output, uint8_t last, uint8_t occurrence, size_t output_len) {
    output[output_len] = occurrence;
    output[output_len + 1] = last;
}

size_t rle(uint8_t *input, size_t input_len, uint8_t **output) {
    assert(input && "ASSERT: Provided null buffer.");
    *output = realloc(*output, input_len);
    size_t output_len = 0;
    
    uint8_t *iter = input; 
    uint8_t last = *input;
    uint8_t occurrence = 0;

    do {
        if (*iter == '\n') {
            continue;
        }

        if (last != *iter) {
            memory_resize_if_needed(*output, output_len + 2, input_len);
            encode_block(*output, last, occurrence, output_len);
            occurrence = 0;
            output_len += 2;
        } 
        
        ++occurrence;
        last = *iter;
    } while (*iter++ != '\0');

    return output_len;
}
