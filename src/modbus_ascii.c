#include "modbus_ascii.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modbus_utils.h"

/*
* buff: |------ascii hex data------|--ascii hex LRC--|--CR-LF--|
*       ^         n bytes                2 bytes       2 bytes      // bytes of character
*       |2B slaveAddr|2B funcCode|4B regAddr|...|
*
* u8Data.data: |--slaveAddr--|--functionCode--|--reg(coil)Addr--|--funcCodeDependantData--|
*                 1 bytes        1 bytes          2 bytes               n bytes
*/

MbErr mb_check_packet(char* buff) {
    // skip the ':'
    buff++;

    size_t buff_size = strlen(buff);

    int16_t slave_addr;
    int16_t function_code;
    int16_t register_addr;
    uint8_t lrc;
    uint8_t dummy_lrc;
    MbErr err;

    uint8_t u8_data[32];
    size_t data_size = 0;

    char answer[64];
    size_t answer_size = 0;

    // strip the LRC; CR and LF are none existant because we have striped theme before passing to this function.
    size_t hex_data_size = buff_size - 2;

    lrc = strtol(buff + hex_data_size, NULL, 16);
    asciihex_to_int(buff, u8_data, hex_data_size);
    data_size += hex_data_size / 2;

    dummy_lrc = chksum8(u8_data, data_size);

    slave_addr = u8_data[0];
    function_code = u8_data[1];
    register_addr = (uint16_t)u8_data[2] << 8 | u8_data[3];

    // thesting code -> :07030010000ADC
    if (lrc != dummy_lrc) {
        return MB_LRCFAIL;
    }

    if (slave_addr != DEVICE_ADDRESS) {
        return MB_ADDFAIL;
    }

    switch (function_code) {
        case R_HoldingRegisters: {
            // extracting data from int data arr of request
            //---------------------------------------------------------------------------------------------------------
            uint16_t reg_qouantity = (uint16_t)u8_data[4] << 8 | u8_data[5];
            printf("q: %d\n", reg_qouantity);
            //---------------------------------------------------------------------------------------------------------

            // actions
            //---------------------------------------------------------------------------------------------------------
            //---------------------------------------------------------------------------------------------------------

            // building int data arr of answer from index 2 (two first bytes are always the same as the request)
            // (overwriting the same buffer)
            //---------------------------------------------------------------------------------------------------------
            u8_data[2] = (uint8_t)reg_qouantity;
            data_size = 3;
            for (size_t i = 0; i < reg_qouantity; i++) {
                u8_data[3 + i * 2] = (uint8_t)(mbholding_register[register_addr + i] >> 8);
                u8_data[4 + i * 2] = (uint8_t)(mbholding_register[register_addr + i]);
            }
            // 1B slAddr + 1B fnCode + 1B bCount + (regQuantity*2)B data(8bit Hi & Lo)
            data_size += reg_qouantity * 2;
            dummy_lrc = chksum8(u8_data, data_size);
            u8_data[data_size] = dummy_lrc;
            data_size += 1;
            //---------------------------------------------------------------------------------------------------------

            // building the actual ascii hex answer
            //---------------------------------------------------------------------------------------------------------
            answer[0] = ':';
            answer_size += 1;
            int_to_asciihex(u8_data, answer + answer_size, data_size);
            answer_size += data_size * 2;
            answer[answer_size] = '\r';
            answer_size += 1;
            answer[answer_size] = '\n';
            answer_size += 1;
            answer[answer_size] = '\0';
            //---------------------------------------------------------------------------------------------------------
        } break;
        case W_SingleRegister: {
            // extracting data from int data arr of request
            //---------------------------------------------------------------------------------------------------------
            uint16_t write_short_data = (uint16_t)u8_data[4] << 8 | u8_data[5];
            //---------------------------------------------------------------------------------------------------------

            // actions
            //---------------------------------------------------------------------------------------------------------
            mbholding_register[register_addr] = write_short_data;
            //---------------------------------------------------------------------------------------------------------

            // building int data arr of answer from index 2 (two first bytes are always the same as the request)
            //---------------------------------------------------------------------------------------------------------
            //---------------------------------------------------------------------------------------------------------

            // building the actual ascii hex answer
            //---------------------------------------------------------------------------------------------------------
            memcpy(answer, buff - 1, buff_size + 1);
            answer_size = buff_size + 1;
            answer[answer_size] = '\0';
            answer_size += 1;
            //---------------------------------------------------------------------------------------------------------
        } break;
        case W_MultipleRegisters: {
            // extracting data from int data arr of request
            //---------------------------------------------------------------------------------------------------------
            uint16_t reg_quantity = (uint16_t)u8_data[4] << 8 | u8_data[5];
            uint8_t byte_count = u8_data[6];
            //---------------------------------------------------------------------------------------------------------

            // actions
            //---------------------------------------------------------------------------------------------------------
            for (size_t i = 0; i < reg_quantity; i++) {
                // *((uint8_t *)(&mbholding_register[registerAddr + i]) + 1) = u8Data[7 + i*2];
                // *((uint8_t *)(&mbholding_register[registerAddr + i])    ) = u8Data[8 + i*2];
                mbholding_register[register_addr + i] =
                    (uint16_t)u8_data[7 + i * 2] << 8 | u8_data[8 + i * 2];
            }
            //---------------------------------------------------------------------------------------------------------

            // building int data arr of answer from index 2 (two first bytes are always the same as the request)
            //---------------------------------------------------------------------------------------------------------
            data_size = 6;
            dummy_lrc = chksum8(u8_data, data_size);
            u8_data[data_size] = dummy_lrc;
            data_size += 1;
            //---------------------------------------------------------------------------------------------------------

            // building the actual ascii hex answer
            //---------------------------------------------------------------------------------------------------------
            answer[0] = ':';
            answer_size += 1;
            int_to_asciihex(u8_data, answer + answer_size, data_size);
            answer_size += data_size * 2;
            answer[answer_size] = '\r';
            answer_size += 1;
            answer[answer_size] = '\n';
            answer_size += 1;
            answer[answer_size] = '\0';
            //---------------------------------------------------------------------------------------------------------
        } break;
        default: {
            return MB_UNSUPFUNC;
        } break;
    }
    printf("%s", answer);
    return MB_ACCEP;
}

void set_register_i16(uint16_t addr, int16_t value) {
    uint16_t* p = (uint16_t*)&value;
    mbholding_register[addr] = *p;
}

void set_register_i32(uint16_t addr, int32_t value) {
    uint16_t* p = (uint16_t*)&value;
    mbholding_register[addr + 1] = *(p + 1);
    mbholding_register[addr] = *(p);
}
void set_register_f32(uint16_t addr, float value) {
    uint16_t* p = (uint16_t*)&value;
    mbholding_register[addr + 1] = *(p + 1);
    mbholding_register[addr] = *(p);
}

int16_t read_register_i16(uint16_t addr) {
    int16_t* p = (int16_t*)(&(mbholding_register[addr]));
    return *p;
}

int32_t read_register_i32(uint16_t addr) {
    int32_t* p = (int32_t*)(&(mbholding_register[addr]));
    return *p;
}

float read_register_f32(uint16_t addr) {
    float* p = (float*)(&(mbholding_register[addr]));
    return *p;
}

int main(int argc, char* argv[]) {
    char* input = *(argv + 1);
    MbErr err = mb_check_packet(input);
    return err;
}
