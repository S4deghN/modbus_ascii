#ifndef mbASCIIslave_h
#define mbASCIIslave_h

#include <stdint.h>
#include <stdlib.h>

#define R_Coils                01  //01 (0x01)
#define R_DiscreteInputs       02  //02 (0x02)
#define R_HoldingRegisters     03  //03 (0x03)
#define R_InputRegisters       04  //04 (0x04)
#define W_SingleCoil           05  //05 (0x05)
#define W_SingleRegister       06  //06 (0x06)
#define W_MultipleCoils        15  //15 (0x0F)
#define W_MultipleRegisters    16  //16 (0x10)
#define RW_MultipleRegisters   23  //23 (0x17)
#define R_DeviceIdentification 43  //43 (0x2B)

#define MODBUS_HEADER   58  // char ':'
#define LINEFEED        10
#define CARRIAGE_RETURN 13
#define ASCCI2DEC(x)    (x - 48)
#define ASCII2NUM(x)    (x - 48)
#define REG_SIZE        254

/*-------------------- Here se the modbus address between 2 -> 9 ---------------------------*/
#define DEVICE_ADDRESS 7
/*------------------------------------------------------------------------------------------*/

static uint16_t mbholding_register[REG_SIZE] = {0};

typedef enum {
    MB_ACCEP = 0,  // packet accepted.
    MB_SRTFAIL,    // illegal start of packet.
    MB_ADDFAIL,    // illegal device address.
    MB_LRCFAIL,    // LRC check failed.
    MB_UNSUPFUNC   // unsupported function codes
} MbErr;

// void mb_poll(HardwareSerial* hmiSer, HardwareSerial* debSer = NULL);
void set_register_i16(uint16_t addr, int16_t value);
void set_register_i32(uint16_t addr, int32_t value);
void set_register_f32(uint16_t addr, float value);
int16_t read_register_i16(uint16_t addr);
int32_t read_register_i32(uint16_t addr);
float read_register_f32(uint16_t addr);

#endif
