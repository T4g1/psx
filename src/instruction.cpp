#include "instruction.h"

#include <iostream>

#include "common.h"
#include "log.h"

using namespace std;


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


void decode(char* buffer, size_t size, uint32_t data)
{
    uint8_t opcode = get_primary_opcode(data);
    uint8_t opcode_special = get_secondary_opcode(data);
    uint8_t cop_opcode = get_cop_opcode(data);              // Bits 25 - 21
    uint8_t sec_opcode = get_secondary_opcode(data);    // Bits 5 - 0

    switch(opcode) {
    case 0x00:
        switch(opcode_special) {
        case 0x00:
            snprintf(buffer, size, "SLL $rt%zu, $rd%zu, %u", get_rt(data), get_rd(data), get_imm5(data));
            break;
        case 0x02:
            snprintf(buffer, size, "SRL $rt%zu, $rd%zu, %u", get_rt(data), get_rd(data), get_imm5(data));
            break;
        case 0x03:
            snprintf(buffer, size, "SRA $rt%zu, $rd%zu, %u", get_rt(data), get_rd(data), get_imm5(data));
            break;
        case 0x04:
            snprintf(buffer, size, "SLLV $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x06:
            snprintf(buffer, size, "SRLV $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x07:
            snprintf(buffer, size, "SRAV $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x08:
            snprintf(buffer, size, "JR $rt%zu", get_rs(data));
            break;
        case 0x09:
            snprintf(buffer, size, "JALR $rt%zu, $rd%zu", get_rs(data), get_rd(data));
            break;
        case 0x0C:
            snprintf(buffer, size, "SYSCALL");
            break;
        case 0x0D:
            snprintf(buffer, size, "BREAK");
            break;
        case 0x10:
            snprintf(buffer, size, "MFHI $rd%zu", get_rd(data));
            break;
        case 0x11:
            snprintf(buffer, size, "MTHI $rs%zu", get_rs(data));
            break;
        case 0x12:
            snprintf(buffer, size, "MFLO $rd%zu", get_rd(data));
            break;
        case 0x13:
            snprintf(buffer, size, "MTLO $rs%zu", get_rs(data));
            break;
        case 0x18:
            snprintf(buffer, size, "MULT $rs%zu, $rt%zu", get_rs(data), get_rt(data));
            break;
        case 0x19:
            snprintf(buffer, size, "MULTU $rs%zu, $rt%zu", get_rs(data), get_rt(data));
            break;
        case 0x1A:
            snprintf(buffer, size, "DIV $rs%zu, $rt%zu", get_rs(data), get_rt(data));
            break;
        case 0x1B:
            snprintf(buffer, size, "DIVU $rs%zu, $rt%zu", get_rs(data), get_rt(data));
            break;
        case 0x20:
            snprintf(buffer, size, "ADD $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x21:
            snprintf(buffer, size, "ADDU $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x22:
            snprintf(buffer, size, "SUB $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x23:
            snprintf(buffer, size, "SUBU $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x24:
            snprintf(buffer, size, "AND $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x25:
            snprintf(buffer, size, "OR $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x26:
            snprintf(buffer, size, "XOR $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x27:
            snprintf(buffer, size, "NOR $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x2A:
            snprintf(buffer, size, "SLT $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        case 0x2B:
            snprintf(buffer, size, "SLTU $rs%zu, $rt%zu, $rd%zu", get_rs(data), get_rt(data), get_rd(data));
            break;
        default: snprintf(
                buffer, size, "EXCEPTION ILLEGAL");
            break;
        }
        break;
    case 0x01:
        snprintf(buffer, size, "BcondZ $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x02:
        snprintf(buffer, size, "J %d", get_imm26(data));
        break;
    case 0x03:
        snprintf(buffer, size, "JAL %d", get_imm26(data));
        break;
    case 0x04:
        snprintf(buffer, size, "BEQ $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x05:
        snprintf(buffer, size, "BNE $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x06:
        snprintf(buffer, size, "BLEZ $rs%zu, %d", get_rs(data), get_imm16_se(data));
        break;
    case 0x07:
        snprintf(buffer, size, "BGTZ $rs%zu, %d", get_rs(data), get_imm16_se(data));
        break;
    case 0x08:
        snprintf(buffer, size, "ADDI $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x09:
        snprintf(buffer, size, "ADDIU $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x0A:
        snprintf(buffer, size, "SLTI $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x0B:
        snprintf(buffer, size, "SLTIU $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x0C:
        snprintf(buffer, size, "ANDI $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16(data));
        break;
    case 0x0D:
        snprintf(buffer, size, "ORI $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16(data));
        break;
    case 0x0E:
        snprintf(buffer, size, "XORI $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16(data));
        break;
    case 0x0F:
        snprintf(buffer, size, "LUI $rt%zu, %u", get_rt(data), get_imm16(data));
        break;
    case 0x10:
        switch(cop_opcode) {
        case 0b00000:
            snprintf(buffer, size, "MFC0 $rt%zu, $rd%zu", get_rt(data), get_rd(data));
            break;
        case 0b00100:
            snprintf(buffer, size, "MTC0 $rt%zu, $rd%zu", get_rt(data), get_rd(data));
            break;
        case 0b10000:
            switch(sec_opcode) {
            case 0b010000:
                snprintf(buffer, size, "RFE");
                break;
            default: snprintf(
                    buffer, size, "COP0 Invalid");
                break;
            };
            break;
        default: snprintf(
                buffer, size, "COP0 Invalid");
            break;
        };
        break;
    case 0x11:
        snprintf(buffer, size, "COP1");
        break;
    case 0x12:
        snprintf(buffer, size, "COP2 GTE");
        break;
    case 0x13:
        snprintf(buffer, size, "COP3");
        break;
    case 0x20:
        snprintf(buffer, size, "LB $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x21:
        snprintf(buffer, size, "LH $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x22:
        snprintf(buffer, size, "LWL $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x23:
        snprintf(buffer, size, "LW $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x24:
        snprintf(buffer, size, "LBU $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x25:
        snprintf(buffer, size, "LHU $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x26:
        snprintf(buffer, size, "LWR $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x28:
        snprintf(buffer, size, "SB $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x29:
        snprintf(buffer, size, "SH $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x2A:
        snprintf(buffer, size, "SWL $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x2B:
        snprintf(buffer, size, "SW $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x2E:
        snprintf(buffer, size, "SWR $rs%zu, $rt%zu, %d", get_rs(data), get_rt(data), get_imm16_se(data));
        break;
    case 0x30:
        snprintf(buffer, size, "LWC0");
        break;
    case 0x31:
        snprintf(buffer, size, "LWC1");
        break;
    case 0x32:
        snprintf(buffer, size, "LWC2 GTE");
        break;
    case 0x33:
        snprintf(buffer, size, "LWC2");
        break;
    case 0x38:
        snprintf(buffer, size, "SWC0");
        break;
    case 0x39:
        snprintf(buffer, size, "SWC1");
        break;
    case 0x3A:
        snprintf(buffer, size, "SWC2 GTE");
        break;
    case 0x3B:
        snprintf(buffer, size, "SWC3");
        break;
    default: snprintf(
            buffer, size, "EXCEPTION ILLEGAL");
        break;
    }
}
