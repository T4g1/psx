#include "instruction.h"


uint32_t extract(uint32_t data, size_t from, size_t size)
{
    uint32_t mask = (1 << size) - 1;

    return (data >> from) & mask;
}


uint8_t get_primary_opcode(uint32_t instruction)
{
    return extract(instruction, 26, 6);
}


uint8_t get_secondary_opcode(uint32_t instruction)
{
    return extract(instruction, 0, 5);
}


size_t get_rs(uint32_t instruction)
{
    return extract(instruction, 21, 5);
}


size_t get_rt(uint32_t instruction)
{
    return extract(instruction, 16, 5);
}


size_t get_rd(uint32_t instruction)
{
    return extract(instruction, 11, 5);
}


uint8_t get_imm5(uint32_t instruction)
{
    return extract(instruction, 6, 5);
}


uint16_t get_imm16(uint32_t instruction)
{
    return extract(instruction, 0, 16);
}


uint32_t get_imm26(uint32_t instruction)
{
    return extract(instruction, 0, 26);
}


uint32_t get_comment(uint32_t instruction)
{
    return extract(instruction, 6, 20);
}


