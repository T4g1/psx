#include "instruction.h"

#include "common.h"


uint8_t get_primary_opcode(uint32_t instruction)
{
    return extract(instruction, 26, 6);
}


uint8_t get_secondary_opcode(uint32_t instruction)
{
    return extract(instruction, 0, 6);
}


uint8_t get_cop_opcode(uint32_t instruction)
{
    return extract(instruction, 21, 5);
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


/**
 * @brief      Same as get_imm16 but gets signed value
 * Signed value is the same a 16bit value but padded with MSB on the left
 */
uint32_t get_imm16_se(uint32_t instruction)
{
    int16_t value = extract(instruction, 0, 16) & 0xFFFF;

    return (uint32_t)value;
}


uint32_t get_imm26(uint32_t instruction)
{
    return extract(instruction, 0, 26);
}


uint32_t get_comment(uint32_t instruction)
{
    return extract(instruction, 6, 20);
}


