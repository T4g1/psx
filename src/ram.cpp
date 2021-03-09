#include "ram.h"

#include <cstring>


RAM::~RAM()
{
}


/**
 * @brief      Initialize the RAM
 * @return     true in case of success, false otherwise
 */
bool RAM::init()
{
    memset(data, POISON_VALUE, RAM_SIZE);

    return true;
}
