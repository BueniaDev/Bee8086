/*
    This file is part of the Bee8086 engine.
    Copyright (C) 2021 BueniaDev.

    Bee8086 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bee8086 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bee8086.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef BEE8086_H
#define BEE8086_H

#include <iostream>
#include <sstream>
#include <cstdint>
using namespace std;
namespace bee8086
{
    // Define the Bee8086 class here to avoid compilation errors
    // about undeclared class variables in the interface class
    class Bee8086;

    // Interface between emulated 8086 and any emulated memory/peripherals
    class Bee8086Interface
    {
	public:
	    Bee8086Interface();
	    ~Bee8086Interface();

	    // Reads a byte from memory
	    virtual uint8_t readByte(uint32_t addr) = 0;
	    // Writes a byte to memory
	    virtual void writeByte(uint32_t addr, uint8_t val) = 0;
	    // Reads a byte from an I/O port
	    virtual uint8_t portIn(uint16_t port) = 0;
	    // Writes a byte to an I/O port
	    virtual void portOut(uint16_t port, uint8_t val) = 0;
	    // Condition for manual interrupt override
	    virtual bool isInterruptOverride(uint8_t int_num) = 0;
	    // Function for manual interrupt override
	    virtual void interruptOverride(Bee8086 &state, uint8_t int_num) = 0;
	    // Function for converting segment and offset to physical address
	    virtual uint32_t convertSeg(uint16_t seg, uint16_t offs) = 0;
    };

    // Class for 8086's internal registers
    class Bee8086Register
    {
	public:
	    Bee8086Register();
	    ~Bee8086Register();

	    // Fetches value of 16-bit register
	    uint16_t getreg();
	    // Sets value of 16-bit register
	    void setreg(uint16_t val);

	    // Fetches value of 8-bit high register
	    uint8_t gethi();
	    // Sets value of 8-bit high register
	    void sethi(uint8_t val);

	    // Fetches value of 8-bit low register
	    uint8_t getlo();
	    // Sets value of 8-bit low register
	    void setlo(uint8_t val);

	private:
	    // Private declarations of 8-bit registers
	    uint8_t hi = 0;
	    uint8_t lo = 0;
    };

    // Class for the actual 8086 emulation logic
    class Bee8086
    {
	public:
	    Bee8086();
	    ~Bee8086();

	    // Initializes the CPU
	    // Takes two optional arguments to set the initial values of the instruction pointer
	    // and code segment registers
	    void init(uint16_t init_cs = 0xF000, uint16_t init_ip = 0xFFF0);

	    // Stops the emulated CPU
	    void shutdown();

	    // Resets the emulated CPU
	    void reset(uint16_t init_cs = 0xF000, uint16_t init_ip = 0xFFF0);

	    // Sets a custom interface for the emulated 8086
	    void setinterface(Bee8086Interface *cb);

	    // Runs the CPU for one instruction
	    int runinstruction();

	    // Prints debug output to stdout
	    void debugoutput(bool print_disassembly = true);

	    size_t disassembleinstr(ostream &stream, size_t addr);

	    // Fetches contents of registers
	    uint8_t get_ah(); // AH
	    uint8_t get_al(); // AL
	    uint16_t get_ax(); // AX
	    uint8_t get_ch(); // CH
	    uint8_t get_cl(); // CL
	    uint16_t get_cx(); // CX
	    uint8_t get_dh(); // DH
	    uint8_t get_dl(); // DL
	    uint16_t get_dx(); // DX
	    uint8_t get_bh(); // BH
	    uint8_t get_bl(); // BL
	    uint16_t get_bx(); // BX

	    void set_ah(uint8_t val);
	    void set_al(uint8_t val);

	    // Fetches contents of segment registers and IP
	    uint16_t get_cs();
	    uint16_t get_es();
	    uint16_t get_ip();

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 1) ? true : false;
	    }

	    template<typename T>
	    T setbit(T reg, int bit)
	    {
		return (reg | (1 << bit));
	    }

	    template<typename T>
	    T resetbit(T reg, int bit)
	    {
		return (reg & ~(1 << bit));
	    }

	    template<typename T>
	    T changebit(T reg, int bit, bool val)
	    {
		if (val == true)
		{
		    return setbit(reg, bit);
		}
		else
		{
		    return resetbit(reg, bit);
		}
	    }

	    // Private declaration of interface class
	    Bee8086Interface *inter = NULL;

	    // Register declarations

	    // General-purpose registers
	    Bee8086Register ax; // AX (hi is AH, lo is AL)
	    Bee8086Register bx; // BX (hi is BH, lo is BL)
	    Bee8086Register cx; // CX (hi is CH, lo is CL)
	    Bee8086Register dx; // DX (hi is DH, lo is DL)

	    // Pointer and index registers
	    uint16_t ip; // Instruction pointer (i.e. program counter)
	    uint16_t sp; // Stack pointer
	    uint16_t bp; // Base pointer
	    uint16_t si; // Source index
	    uint16_t di; // Destination index

	    // Segment registers
	    uint16_t cs; // Code segment
	    uint16_t ds; // Data segment
	    uint16_t ss; // Stack segment
	    uint16_t es; // Extra segment

	    // Status register
	    uint16_t status_reg;

	    // Contains the main logic for the 8086 instruction set
	    int executenextopcode(uint8_t opcode);

	    // Prints the unrecognized instruction and then exits
	    void unrecognizedopcode(uint8_t opcode);

	    // Converts segment and offset to physical address
	    uint32_t convertSeg(uint16_t seg, uint16_t offs);

	    // Reads byte from memory
	    uint8_t readByte(uint32_t addr);
	    // Writes byte to memory
	    void writeByte(uint32_t addr, uint8_t val);

	    // Reads 16-bit word from memory
	    uint16_t readWord(uint32_t addr);
	    // Writes 16-bit word to memory
	    void writeWord(uint32_t addr, uint16_t val);

	    // Fetches next byte from memory (and updates the program counter)
	    uint8_t getimmByte();

	    // Fetches next word from memory (and updates the program counter)
	    uint16_t getimmWord();

	    // Reads byte from I/O port
	    uint8_t portIn(uint16_t port);

	    // Reads word from I/O port
	    uint16_t portIn16(uint16_t port);

	    // Writes byte to I/O port
	    void portOut(uint16_t port, uint8_t val);

	    // Writes word to I/O port
	    void portOut16(uint16_t port, uint16_t val);

	    bool isInterruptOverride(uint8_t int_num);
	    void interruptOverride(uint8_t int_num);

	    struct ModRM
	    {
		int mod = 0;
		int reg = 0;
		int mem = 0;
		uint16_t segment = 0;
		uint16_t addr = 0;
	    };

	    struct ModRMDasm
	    {
		int mod = 0;
		int reg = 0;
		int mem = 0;
		uint16_t segment = 0;
		uint16_t addr = 0;
		string dasm_str;
	    };

	    ModRM current_mod_rm;
	    ModRMDasm dasm_mod_rm;

	    bool is_zero()
	    {
		return testbit(status_reg, 6);
	    }

	    void set_zero(bool val)
	    {
		status_reg = changebit(status_reg, 6, val);
	    }

	    bool is_carry()
	    {
		return testbit(status_reg, 0);
	    }

	    void set_carry(bool val)
	    {
		status_reg = changebit(status_reg, 0, val);
	    }

	    bool is_above()
	    {
		return (!is_zero() && !is_carry());
	    }

	    bool is_direction()
	    {
		return testbit(status_reg, 10);
	    }

	    void set_direction(bool val)
	    {
		status_reg = changebit(status_reg, 10, val);
	    }

	    // Helper enum for memory segmentation
	    enum Segment : int
	    {
		Default = 0, // Default
		Code = 1, // Code segment
		Data = 2, // Data segment
		Stack = 3, // Stack segment
		Extra = 4, // Extra segment
	    };

	    Segment mem_segment = Segment::Default;

	    bool is_segment_override = false;
	    bool is_rep = false;

	    #include "instructions.inl"
	    #include "disassembly.inl"
    };
};

#endif // BEE8086_H