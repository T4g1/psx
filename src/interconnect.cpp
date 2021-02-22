#include "log.h"
#include "bios.h"

#include "interconnect.h"

#define BIOS_START  0xBFC00000
#define BIOS_END    BIOS_START + BIOS_SIZE


Interconnect::~Interconnect()
{
}


bool Interconnect::init(BIOS *bios)
{
    this->bios = bios;

    return true;
}


uint32_t Interconnect::load32(uint32_t address)
{
    //debug("[MEM] Fetch from: 0x%08x\n", address);

    // Is it mapped to BIOS ?
    if (BIOS_START <= address && address < BIOS_END) {
        return bios->load32(address - BIOS_START);
    }

    error("Unhandled load32 at 0x%08x", address);
    exit(1);
}
