#include "common.h"


bool in_range(uint32_t address, uint32_t start, size_t size)
{
    return start <= address && address < start + size;
}


uint32_t extract(uint32_t data, size_t from, size_t size)
{
    uint32_t mask = (1 << size) - 1;

    return (data >> from) & mask;
}
