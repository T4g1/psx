#include <fstream>

#include "log.h"
#include "common.h"

#include "bios.h"


BIOS::~BIOS()
{
}


bool BIOS::init(std::string bios_path)
{
    return load_bios(bios_path);
}


bool BIOS::load_bios(std::string path)
{
    std::ifstream file (path, std::fstream::binary);

    // File size
    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);

    if (length != BIOS_SIZE) {
        error("BIOS file size is not %d!", BIOS_SIZE);
        return false;
    }

    file.read((char*)data, BIOS_SIZE * sizeof(uint8_t));

    return true;
}


void BIOS::store32(uint32_t address, uint32_t value)
{
    data[address + 0] = extract(value, 0, 8);
    data[address + 1] = extract(value, 8, 8);
    data[address + 2] = extract(value, 16, 8);
    data[address + 3] = extract(value, 24, 8);
}


/**
 * @brief      Loads a 32.bit data at the given address
 * @param[in]  address  The address
 * @return     Returns the 32bit little endian data from the given address
 */
uint32_t BIOS::load32(uint32_t address)
{
    uint8_t b0 = data[address + 0];
    uint8_t b1 = data[address + 1];
    uint8_t b2 = data[address + 2];
    uint8_t b3 = data[address + 3];

    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}
