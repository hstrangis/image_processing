#include <stdint.h>
#include <stdio.h>

#define MASK_BAJA 0x0000FF
#define MASK_MEDIA 0x00FF00
#define SHIFT_1BYTE 8
#define SHIFT_2BYTE 16

void escribir_int16_little_endian(FILE *f, int16_t v) {

    uint8_t vec_16[2];
    vec_16[0] = MASK_BAJA & v; 
    vec_16[1] = (MASK_MEDIA & v) >> SHIFT_1BYTE;     
    fwrite(vec_16, sizeof(int8_t), 2, f);
}

void escribir_int32_little_endian(FILE *f, int32_t v) {

    uint8_t vec_32[4];
    vec_32[0] = MASK_BAJA & v;
    vec_32[1] = (MASK_MEDIA & v) >> SHIFT_1BYTE;
    vec_32[2] = (v >> SHIFT_2BYTE) & MASK_BAJA;
    vec_32[3] = ((v >> SHIFT_2BYTE) >> SHIFT_1BYTE);
    fwrite(vec_32, sizeof(int8_t), 4, f); 
}