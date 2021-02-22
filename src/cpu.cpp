#include "log.h"
#include "interconnect.h"

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
    PC = DEFAULT_PC;

    return true;
}


/**
 * @brief      Reset the CPU state
 */
void CPU::reset()
{
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
    // TODO
}


void CPU::set_inter(Interconnect* inter)
{
    this->inter = inter;
}
