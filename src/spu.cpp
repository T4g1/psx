#include "spu.h"


SPU::~SPU()
{
}


/**
 * @brief      Initialize the SPU state
 * @return     true in case of success, false otherwise
 */
bool SPU::init()
{
    reset();

    return true;
}


/**
 * @brief      Reset the SPU state
 */
void SPU::reset()
{
}
