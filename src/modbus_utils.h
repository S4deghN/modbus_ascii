#ifndef modbus_utils_h
#define modbus_utils_h

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void asciihex_to_int(const char* asciiBuff, uint8_t* intData, size_t size) {
    char b[2];
    if ((size & 1) == 0) {
        for (int i = 0, j = 0; i + 1 < size; i += 2, j++) {
            b[0] = asciiBuff[i];
            b[1] = asciiBuff[i + 1];

            intData[j] = strtol(b, NULL, 16);
        }
    }
}

void int_to_asciihex(uint8_t* intData, char* asciiBuff, size_t size) {
    for (size_t i = 0, j = 0; i < size; i++, j += 2) {
        sprintf(asciiBuff + j, "%.2X", intData[i]);
    }
}

uint8_t chksum8(const uint8_t* data, size_t len) {
    uint16_t sum = 0;
    while (len--) {
        sum += data[len];
    }
    return (uint8_t)(~sum + 1);
}

#endif
