#include <stdlib.h>

#include "log.h"
#include "interconnect.h"
#include "instruction.h"

#include "cpu.h"


CPU::~CPU()
{
}


/**
 * @brief      Initialize the CPU state
 * @return     true in case of success, false otherwise
 */
bool CPU::init()
{
    reset();

    return true;
}


/**
 * @brief      Reset the CPU state
 */
void CPU::reset()
{
    reg[0] = 0;
    for (size_t i=1; i<REG_COUNT; i++) {
        reg[i] = DEFAULT_REG;
    }

    PC = DEFAULT_PC;
    PC = DEFAULT_PC;
}


void CPU::run_next()
{
    uint32_t instruction = inter->load32(PC);

    PC += INSTRUCTION_LENGTH;

    decode_and_execute(instruction);
}


void CPU::decode_and_execute(uint32_t data)
{
    debug("[CPU] Instruction: 0x%08x\n", data);

    uint8_t opcode = get_primary_opcode(data);

    switch(opcode) {
        case 0x0F: LUI(get_rt(data), get_imm16(data)); break;
        default:
            error("Unhandled OPCODE: 0x%08x", data);
            exit(1);
    }
}


void CPU::set_inter(Interconnect* inter)
{
    this->inter = inter;
}


uint32_t CPU::get_reg(size_t index)
{
    return reg[index];
}


void CPU::set_reg(size_t index, uint32_t value)
{
    reg[index] = value;
    reg[0] = 0;
}


void CPU::LUI(size_t rt, uint16_t imm16)
{
    set_reg(rt, imm16 << 16);
}
