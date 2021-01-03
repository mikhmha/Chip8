#include <chip8.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <util.hpp>

void Chip8::reset() noexcept
{
	pc = 0x200;
	sp = 0;
	I = 0;
	opcode = 0;

	delay_timer = 60;
	sound_timer = 60;

	draw_flag = false;

	display.fill(0);
	memory.fill(0);
	V.fill(0);
	stack.fill(0);
	input.fill(0);

	util::load_fontset(memory);

}



uint8_t Chip8::load_rom(const std::string& path)
{
	auto buffer = util::read_rom(path);

	if (buffer.size() == 0 || buffer.size() > 3232)
	{
		if ( buffer.size() > 3232)
			std::cerr << "ERROR: ROM WAS NOT LOADED! REASON: SIZE TO BIG." << '\n';
		return 0;
	}

	for (auto i = 0; i != buffer.size(); i++)
		memory[i + 0x200] = buffer[i];

	return 1;
}


void Chip8::run()
{
	// fetch
	opcode = memory[pc] << 8 | memory[pc + 1];
	pc += 2;

	// decode and execute
	//std::clog << "EXECUTING: "<< std::hex << opcode << '\n';
	uint8_t i = ((opcode & 0xF000) >> 12) % 0x10;
	Chip8Execute[i](opcode & 0x0FFF);


	// timers
	if (delay_timer > 0)
		--delay_timer;
	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			std::clog << "BEEP!" << '\n';
		--sound_timer;
	}

}

void Chip8::SEL_0(uint8_t addr)
{
	if (addr == 0xE0) { display.fill(0); }
	else if (addr == 0xEE) { pc = stack[--sp]; }
}

void Chip8::SEL_E(uint8_t X, uint8_t addr)
{
	if (addr == 0x9E)
		if (input[V[X]])
			pc += 2;

	if (addr == 0xA1)
		if (!input[V[X]])
			pc += 2;
}


void Chip8::SEL_F0(uint8_t X, uint8_t addr)
{
	if (addr == 0x07) { V[X] = delay_timer;  }
	else if (addr == 0x0A) { _FX0A(X); };

}

void Chip8::SEL_F1(uint8_t X, uint8_t addr)
{
	if (addr == 0x15) { delay_timer = V[X]; }
	else if (addr == 0x18) { sound_timer = V[X];  }
	else if (addr == 0x1E) { I += V[X];  };

}


void Chip8::_CXNN(uint8_t  X, uint8_t NN)
{
	std::srand(std::time(nullptr));
	uint8_t n = std::rand() % 255;
	V[X] = n & NN;
}


void Chip8::_DXYN(uint8_t X, uint8_t Y, uint8_t N)
{
	V[0xF] = 0;

	for (auto i = 0; i != N; i++)
	{
		uint8_t sprite = memory[I + i];
		uint16_t y = ((V[Y] + i) * 64);
		for (auto j = 0; j != 8; j++)
		{
			if (sprite & (0x80 >> j))
			{
				uint16_t x = (V[X] + j);

				uint16_t _index = (y + x) % (64 * 32);
				
				if (display[_index])
					V[0xF] = 1;
				
				display[_index] ^= 255;
			}
		}
	}

	draw_flag = true;
}

void Chip8::_FX0A(uint8_t X)
{

	for (uint8_t i = 0; i != input.size(); i++)
	{
		if (!input[i])
		{
			V[X] = i;
			return;
		}
	}

	pc -= 2;
}

void Chip8::_FX33(uint8_t X)
{
	uint8_t n = V[X];


	memory[I] = n / 100;
	memory[I + 1] = (n / 10) % 10;
	memory[I + 2] = (n % 100) % 10;
}





