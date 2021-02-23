#ifndef COMMON_H
#define COMMON_H

#include  <cstdint>


uint32_t extract(uint32_t data, size_t from, size_t size);
bool in_range(uint32_t address, uint32_t start, size_t size);

#endif /* COMMON_H */
