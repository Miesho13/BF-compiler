#ifndef _RLE_H_
#define _RLE_H_

#include <stdint.h>
#include <stdlib.h>

size_t rle(uint8_t *input, size_t input_len, uint8_t **output);

#endif 
