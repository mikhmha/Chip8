#pragma once
#include <stdint.h>

// OPCODE DECODE FUNCTIONS


inline uint8_t X(uint16_t opcode) noexcept { return ((opcode >>  8) & 0x0F);}
inline uint8_t Y(uint16_t opcode) noexcept { return ((opcode >> 4) & 0x0F); }
