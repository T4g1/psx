#ifndef COMMON_H
#define COMMON_H

#include  <cstdint>

#define MASK_5_BITS         0x1F
#define MASK_6_BITS         0x3F


uint32_t extract(uint32_t data, size_t from, size_t size);
bool in_range(uint32_t address, uint32_t start, size_t size);
void ToggleButton(const char *text, bool *boolean);
void ColorBoolean(bool condition);

#endif /* COMMON_H */
