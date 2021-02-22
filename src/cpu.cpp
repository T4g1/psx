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
    uint32_t instruction = 0; //load32(PC);

    PC += INSTRUCTION_LENGTH;

    decode_and_execute(instruction);
}


void CPU::decode_and_execute(uint32_t data)
{
    // TODO
}
