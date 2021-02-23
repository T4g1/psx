#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include  <cstdint>


uint8_t get_primary_opcode(uint32_t instruction);
uint8_t get_secondary_opcode(uint32_t instruction);
uint8_t get_cop_opcode(uint32_t instruction);
size_t get_rs(uint32_t instruction);
size_t get_rt(uint32_t instruction);
size_t get_rd(uint32_t instruction);
uint8_t get_imm5(uint32_t instruction);
uint16_t get_imm16(uint32_t instruction);
uint32_t get_imm16_se(uint32_t instruction);
uint32_t get_imm26(uint32_t instruction);
uint32_t get_comment(uint32_t instruction);

#endif /* INSTRUCTION_H */
