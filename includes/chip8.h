#pragma once
#include <array>
#include <string>
#include <functional>
#include <decode.hpp>

using ScreenBuffer = std::array<uint8_t, 64 * 32>;
class Chip8
{
public:
	Chip8() { reset(); };
	void reset () noexcept;
	uint8_t load_rom(const std::string &path);
	void run();

	const ScreenBuffer* get_display() const noexcept { return &display; }


	bool draw_flag;
	std::array<uint8_t, 16> input;



private:

	std::array<uint8_t, 4096> memory;
	ScreenBuffer display;

	std::array<uint16_t, 16> stack;
	uint16_t sp;
	
	std::array<uint8_t, 16> V;		// data registers V0 - VF
	uint16_t I;						// address register
	uint16_t opcode;
	uint16_t pc;					

	uint8_t delay_timer;
	uint8_t sound_timer;

	// OPCODES 
	void _1NNN(uint16_t address) { pc = address; };
	void _2NNN(uint16_t address) { stack[sp] = pc; ++sp; pc = address; }
	void _3XNN(uint8_t X, uint8_t NN) { if (V[X] == NN) pc += 2; }
	void _4XNN(uint8_t X, uint8_t NN) { if (V[X] != NN) pc += 2; }
	void _5XY0(uint8_t X, uint8_t Y) { if (V[X] == V[Y]) pc += 2; }
	void _6XNN(uint8_t X, uint8_t NN) { V[X] = NN; }
	void _7XNN(uint8_t X, uint8_t NN) { V[X] += NN; }
	void _9XY0(uint8_t X, uint8_t Y) { if (V[X] != V[Y]) pc += 2; }
	void _ANNN(uint16_t address) { I = address; };
	void _BNNN(uint16_t address) { pc = address + V[0]; }
	void _CXNN(uint8_t  X, uint8_t NN);
	void _DXYN(uint8_t X, uint8_t Y, uint8_t N);
	void _FX0A(uint8_t X);
	void _FX29(uint8_t X) { I = V[X] * 0x5; }
	void _FX33(uint8_t X);
	void _FX55(uint8_t X) { for (auto i = 0; i != X + 1; i++) memory[I + i] = V[i]; }
	void _FX65(uint8_t X) { for (auto i = 0; i != X + 1; i++) V[i] = memory[I + i]; }

	void SEL_0(uint8_t addr);
	void SEL_8(uint8_t X, uint8_t Y, uint8_t S) { ((S - (S % 12)) == 0) ? _8MUX[S](X, Y) : _8MUX[8](X, Y); }
	void SEL_E(uint8_t X, uint8_t addr);
	void SEL_F(uint8_t X, uint8_t Y, uint8_t addr) { _FMUX[Y](X, addr); }
	void SEL_F0(uint8_t X, uint8_t addr);
	void SEL_F1(uint8_t X, uint8_t addr);

	std::array<std::function<void(uint16_t opcode)>, 16> Chip8Execute
	{
		([&](uint16_t opcode) { SEL_0(opcode); }),										// SELECT 
		([&](uint16_t opcode) { _1NNN(opcode); }),										
		([&](uint16_t opcode) { _2NNN(opcode); }),										
		([&](uint16_t opcode) { _3XNN(X(opcode), opcode); }),							
		([&](uint16_t opcode) { _4XNN(X(opcode), opcode); }),							
		([&](uint16_t opcode) { _5XY0(X(opcode), Y(opcode)); }),						
		([&](uint16_t opcode) { _6XNN(X(opcode), opcode); }),							
		([&](uint16_t opcode) { _7XNN(X(opcode), opcode); }),							
		([&](uint16_t opcode) { SEL_8(X(opcode), Y(opcode), opcode & 0x0F); }),			// SELECT
		([&](uint16_t opcode) { _9XY0(X(opcode), Y(opcode)); }),						
		([&](uint16_t opcode) { _ANNN(opcode); }),										
		([&](uint16_t opcode) { _BNNN(opcode); }),										
		([&](uint16_t opcode) { _CXNN(X(opcode), opcode); }),							
		([&](uint16_t opcode) { _DXYN(X(opcode), Y(opcode), opcode & 0x0F); }),			// DRAW
		([&](uint16_t opcode) { SEL_E(X(opcode), opcode); }),							// SELECT
		([&](uint16_t opcode) { SEL_F(X(opcode), Y(opcode), opcode); })					// SELECT
	};



	std::array<std::function<void(uint8_t X, uint8_t Y)>, 9> _8MUX
	{
		 ([&](uint8_t X, uint8_t Y) {V[X] = V[Y]; }),														// 8XY0
		 ([&](uint8_t X, uint8_t Y) {V[X] = V[X] | V[Y]; }),												// 8XY1
		 ([&](uint8_t X, uint8_t Y) {V[X] = V[X] & V[Y]; }),												// 8XY2
		 ([&](uint8_t X, uint8_t Y) {V[X] = V[X] ^ V[Y]; }),												// 8XY3
		 ([&](uint8_t X, uint8_t Y) {V[0xF] = (V[X] > (0xFF - V[Y])) ? 1 : 0; V[X] += V[Y]; }),				// 8XY4
		 ([&](uint8_t X, uint8_t Y) {V[0xF] = (V[Y] > V[X]) ? 0 : 1; V[X] -= V[Y];  }),						// 8XY5
		 ([&](uint8_t X, uint8_t Y) {V[0xF] = V[X] & 0x1;  V[X] >>= 1; }),									// 8XY6
		 ([&](uint8_t X, uint8_t Y) {V[X] = V[0xF] = (V[X] > V[Y]) ? 0 : 1; V[X] = V[Y] - V[X]; }),			// 8XY7
		 ([&](uint8_t X, uint8_t Y) {V[0xF] = V[X] >> 7;  V[X] <<= 1; })									// 8XYE
	};					


	std::array<std::function<void(uint8_t X, uint8_t ADDR)>, 7> _FMUX
	{
		([&](uint8_t X, uint8_t ADDR) { SEL_F0(X, ADDR); }),
		([&](uint8_t X, uint8_t ADDR) { SEL_F1(X, ADDR); }),
		([&](uint8_t X, uint8_t ADDR) { if (ADDR == 0x29) { _FX29(X); }}),
		([&](uint8_t X, uint8_t ADDR) { if (ADDR == 0x33) { _FX33(X); }}),
		([&](uint8_t X, uint8_t ADDR) { return; }),
		([&](uint8_t X, uint8_t ADDR) { if (ADDR == 0x55) { _FX55(X); }}),
		([&](uint8_t X, uint8_t ADDR) { if (ADDR == 0x65) { _FX65(X); }})
	};


};


