#include "ram.h"

#include "common.h"

#include <cstring>

#define POISON_VALUE        0xCA


RAM::~RAM()
{
}


/**
 * @brief      Initialize the RAM
 * @return     true in case of success, false otherwise
 */
bool RAM::init()
{
    memset(data, POISON_VALUE, RAM_SIZE);

    return true;
}


void RAM::store8(uint32_t address, uint8_t value)
{
    data[address] = value;
}


void RAM::store16(uint32_t address, uint16_t value)
{
    data[address + 0] = extract(value, 0, 8);
    data[address + 1] = extract(value, 8, 8);
}


void RAM::store32(uint32_t address, uint32_t value)
{
    data[address + 0] = extract(value, 0, 8);
    data[address + 1] = extract(value, 8, 8);
    data[address + 2] = extract(value, 16, 8);
    data[address + 3] = extract(value, 24, 8);
}


uint8_t RAM::load8(uint32_t address)
{
    return data[address];
}


uint16_t RAM::load16(uint32_t address)
{
    uint8_t b0 = data[address + 0];
    uint8_t b1 = data[address + 1];

    return b0 | (b1 << 8);
}


/**
 * @brief      Loads a 32.bit data at the given address
 * @param[in]  address  The address
 * @return     Returns the 32bit little endian data from the given address
 */
uint32_t RAM::load32(uint32_t address)
{
    uint8_t b0 = data[address + 0];
    uint8_t b1 = data[address + 1];
    uint8_t b2 = data[address + 2];
    uint8_t b3 = data[address + 3];

    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}
