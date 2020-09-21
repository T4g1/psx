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
    PC = 0;

    return true;
}


/**
 * @brief      Reset the CPU state
 */
void CPU::reset()
{
    PC = 0;
}


void CPU::run_next()
{
    // TODO: Fetch instruction

    PC += INSTRUCTION_LENGTH;

    decode_and_execute(0);
}


void CPU::decode_and_execute(uint32_t data)
{
    // TODO
}
