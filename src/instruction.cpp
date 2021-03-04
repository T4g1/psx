#include "instruction.h"

#include "common.h"
#include "log.h"


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
int32_t get_imm16_se(uint32_t instruction)
{
    int16_t value = get_imm16(instruction) & 0xFFFF;

    return (int32_t) value;
}


uint32_t get_imm26(uint32_t instruction)
{
    return extract(instruction, 0, 26);
}


uint32_t get_comment(uint32_t instruction)
{
    return extract(instruction, 6, 20);
}


void decode(uint32_t data)
{
    uint8_t opcode = get_primary_opcode(data);
    uint8_t opcode_special = get_secondary_opcode(data);

    switch(opcode) {
    case 0x00:
        switch(opcode_special) {
        case 0x00: info("SLL $rt%zu, $rd%zu, %u\n", get_rt(data), get_rd(data), get_imm5(data)); break;
        case 0x02: info("SRL $rt%zu, $rd%zu, %u\n", get_rt(data), get_rd(data), get_imm5(data)); break;
        case 0x03: info("SRA $rt%zu, $rd%zu, %u\n", get_rt(data), get_rd(data), get_imm5(data)); break;
        case 0x08: info("JR $rt%zu\n", get_rs(data)); break;
        case 0x09: info("JALR $rt%zu, $rd%zu\n", get_rs(data), get_rd(data)); break;
        case 0x10: info("MFHI $rd%zu\n", get_rd(data)); break;
        case 0x12: info("MFLO $rd%zu\n", get_rd(data)); break;
        case 0x1A: info("DIV $rs%zu, $rt%zu\n", get_rs(data), get_rt(data)); break;
        case 0x1B: info("DIVU $rs%zu, $rt%zu\n", get_rs(data), get_rt(data)); break;
        case 0x20: info("ADD $rs%zu, $rt%zu, $rd%zu\n", get_rs(data), get_rt(data), get_rd(data)); break;
        case 0x21: info("ADDU $rs%zu, $rt%zu, $rd%zu\n", get_rs(data), get_rt(data), get_rd(data)); break;
        case 0x23: info("SUBU $rs%zu, $rt%zu, $rd%zu\n", get_rs(data), get_rt(data), get_rd(data)); break;
        case 0x24: info("AND $rs%zu, $rt%zu, $rd%zu\n", get_rs(data), get_rt(data), get_rd(data)); break;
        case 0x25: info("OR $rs%zu, $rt%zu, $rd%zu\n", get_rs(data), get_rt(data), get_rd(data)); break;
        case 0x2A: info("SLT $rs%zu, $rt%zu, $rd%zu\n", get_rs(data), get_rt(data), get_rd(data)); break;
        case 0x2B: info("SLTU $rs%zu, $rt%zu, $rd%zu\n", get_rs(data), get_rt(data), get_rd(data)); break;
        default: info("SPECIAL N/A\n"); break;
        }
        break;
    case 0x01: info("BcondZ $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x02: info("J %d\n", get_imm26(data)); break;
    case 0x03: info("JAL %d\n", get_imm26(data)); break;
    case 0x04: info("BEQ $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x05: info("BNE $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x06: info("BLEZ $rs%zu, %d\n", get_rs(data), get_imm16_se(data)); break;
    case 0x07: info("BGTZ $rs%zu, %d\n", get_rs(data), get_imm16_se(data)); break;
    case 0x08: info("ADDI $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x09: info("ADDIU $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x0A: info("SLTI $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x0B: info("SLTIU $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x0C: info("ANDI $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16(data)); break;
    case 0x0D: info("ORI $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16(data)); break;
    case 0x0F: info("LUI$rt%zu, %u\n", get_rt(data), get_imm16(data)); break;
    case 0x10: info("TODO COP0\n"); break;
    case 0x11: info("TODO COP1\n"); break;
    case 0x12: info("TODO COP2\n"); break;
    case 0x13: info("TODO COP3\n"); break;
    case 0x20: info("LB $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x23: info("LW $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x24: info("LBU $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x28: info("SB $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x29: info("SH $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x2B: info("SW $rs%zu, $rt%zu, %d\n", get_rs(data), get_rt(data), get_imm16_se(data)); break;
    default: info("N/A\n"); break;
    }
}
