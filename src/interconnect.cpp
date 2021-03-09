#include "interconnect.h"


const uint32_t REGION_MASK[] = {
    // KUSEG: 2048MB
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    // KSEG0: 512MB
    0x7FFFFFFF,
    // KSEG1: 512MB
    0x1FFFFFFF,
    // KSEG2: 1024MB
    0xFFFFFFFF, 0xFFFFFFFF
};


uint32_t mask_region(uint32_t address)
{
    return address & REGION_MASK[address >> 29];
}


Interconnect::~Interconnect()
{
}


bool Interconnect::init(SPU *spu, BIOS *bios, RAM *ram)
{
    this->spu = spu;
    this->bios = bios;
    this->ram = ram;

    return true;
}


bool Interconnect::canLoad32(uint32_t address)
{
    address = mask_region(address);

    // Unaligned memory access should be handled differently
    if (address % 4 != 0) {
        return false;
    }

    // Is it mapped to RAM ?
    if (in_range(address, RAM_START, RAM_SIZE)) {
        return true;
    }

    // Is it mapped to BIOS ?
    else if (in_range(address, BIOS_START, BIOS_SIZE)) {
        return true;
    }

    // Is it mapped to SPU ?
    else if (in_range(address, SPU_START, SPU_SIZE)) {
        return true;
    }

    // Is it mapped to GPU ?
    else if (in_range(address, GPU_START, GPU_SIZE)) {
        return true;
    }

    // Is it mapped to DMA ?
    else if (in_range(address, DMA_START, DMA_SIZE)) {
        return true;
    }

    // IRQ_CONTROL register
    else if (in_range(address, IRQ_CONTROL_START, IRQ_CONTROL_SIZE)) {
        return true;
    }

    return false;
}
