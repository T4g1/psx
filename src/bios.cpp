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
