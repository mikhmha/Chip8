#pragma once
#include <array>
#include <vector>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include "RootDir.h"

namespace util
{

	inline void load_fontset(std::array<uint8_t, 4096>& m) noexcept
	{
		constexpr std::array<uint8_t, 80> chip8_fontset
		{
		  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		  0x20, 0x60, 0x20, 0x20, 0x70, // 1
		  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};

		for (auto i = 0; i != chip8_fontset.size(); i++)
			m[i] = chip8_fontset[i];

	}

	inline std::vector<uint8_t> read_rom(const std::string& path)
	{
		const std::string file_name = ROOT_DIR + path;
		std::ifstream romFile{ file_name, std::ios::binary };

		if (!romFile)
		{
			std::cerr << "ERROR: ROM WAS NOT FOUND! MAKE SURE ROM IS IN THE ROMS FOLDER." << "\n\n";
			return {};
		}

		else
			return std::vector<uint8_t>{std::istreambuf_iterator<char>(romFile), {}};

	}
}