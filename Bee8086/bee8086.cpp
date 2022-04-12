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

#include "bee8086.h"
using namespace bee8086;
using namespace std;

// Constructor/deconstructor definitions for Bee8086Interface
Bee8086Interface::Bee8086Interface()
{

}

Bee8086Interface::~Bee8086Interface()
{

}

// Function declarations for Bee8086Register
Bee8086Register::Bee8086Register()
{

}

Bee8086Register::~Bee8086Register()
{

}

// Functions for the 16-bit "register pair"
// The high register makes up the upper 8 bits of the register pair,
// while the low register makes up the lower 8 bits

uint16_t Bee8086Register::getreg()
{
    return ((hi << 8) | lo);
}

void Bee8086Register::setreg(uint16_t val)
{
    hi = (val >> 8);
    lo = (val & 0xFF);
}

// Functions for the 8-bit registers themselves

// High register
uint8_t Bee8086Register::gethi()
{
    return hi;
}

void Bee8086Register::sethi(uint8_t val)
{
    hi = val;
}

// Low register
uint8_t Bee8086Register::getlo()
{
    return lo;
}

void Bee8086Register::setlo(uint8_t val)
{
    lo = val;
}

// Class definitions for the Bee8086 class
Bee8086::Bee8086()
{

}

Bee8086::~Bee8086()
{

}

// Initialize the emulated 8086
void Bee8086::init(uint16_t init_cs, uint16_t init_pc)
{
    // Initialize the registers (except for CS and PC) to 0
    ax.setreg(0x0000);
    bx.setreg(0x0000);
    cx.setreg(0x0000);
    dx.setreg(0x0000);
    sp = 0x0000;
    bp = 0x0000;
    si = 0x0000;
    di = 0x0000;
    ds = 0x0000;
    ss = 0x0000;
    es = 0x0000;

    // Initialize the CS and PC to the values of init_cs and init_pc, respectively
    cs = init_cs;
    ip = init_pc;

    mem_segment = Segment::Default;

    status_reg = 0;

    // Notify the user that the emulated 8080 has been initialized
    cout << "Bee8086::Initialized" << endl;
}

// Shutdown the emulated 8086
void Bee8086::shutdown()
{
    // Set the interface pointer to NULL if we haven't done so already
    if (inter != NULL)
    {
	inter = NULL;
    }

    // Notify the user that the emulated 8080 has been shut down
    cout << "Bee8086::Shutting down..." << endl;
}

// Reset the emulated 8086
void Bee8086::reset(uint16_t init_cs, uint16_t init_pc)
{
    cout << "Bee8086::Resetting..." << endl;
    init(init_cs, init_pc);
}

// Set callback interface
void Bee8086::setinterface(Bee8086Interface *cb)
{
    // Sanity check to prevent a possible buffer overflow
    // from a erroneous null pointer
    if (cb == NULL)
    {
	cout << "Error: new interface is NULL" << endl;
	return;
    }

    inter = cb;
}

// Print debug output to screen
void Bee8086::debugoutput(bool print_disassembly)
{
    cout << "AX: " << hex << int(ax.getreg()) << endl;
    cout << "CX: " << hex << int(cx.getreg()) << endl;
    cout << "DX: " << hex << int(dx.getreg()) << endl;
    cout << "BX: " << hex << int(bx.getreg()) << endl;

    cout << "SI: " << hex << int(si) << endl;
    cout << "DI: " << hex << int(di) << endl;
    cout << "BP: " << hex << int(bp) << endl;
    cout << "SP: " << hex << int(sp) << endl;

    cout << "IP: " << hex << int(ip) << endl;

    cout << "CS: " << hex << int(cs) << endl;
    cout << "DS: " << hex << int(ds) << endl;
    cout << "ES: " << hex << int(es) << endl;
    cout << "SS: " << hex << int(ss) << endl;

    cout << "Flags: " << hex << int(status_reg) << endl;

    if (print_disassembly)
    {
	stringstream dasm_str;
	disassembleinstr(dasm_str, convertSeg(cs, ip));
	cout << "Current instruction: " << dasm_str.str() << endl;
    }

    cout << endl;
}

// Fetches AH register
uint8_t Bee8086::get_ah()
{
    return ax.gethi();
}

// Sets AH register
void Bee8086::set_ah(uint8_t val)
{
    ax.sethi(val);
}

// Fetches AL register
uint8_t Bee8086::get_al()
{
    return ax.getlo();
}

// Sets AL register
void Bee8086::set_al(uint8_t val)
{
    ax.setlo(val);
}

// Sets carry flag
void Bee8086::set_cf(bool val)
{
    set_carry(val);
}

// Fetches AX register
uint16_t Bee8086::get_ax()
{
    return ax.getreg();
}

// Fetches CH register
uint8_t Bee8086::get_ch()
{
    return cx.gethi();
}

// Fetches CL register
uint8_t Bee8086::get_cl()
{
    return cx.getlo();
}

// Fetches CX register
uint16_t Bee8086::get_cx()
{
    return cx.getreg();
}

// Fetches DH register
uint8_t Bee8086::get_dh()
{
    return dx.gethi();
}

// Fetches DL register
uint8_t Bee8086::get_dl()
{
    return dx.getlo();
}

// Fetches DX register
uint16_t Bee8086::get_dx()
{
    return bx.getreg();
}

// Fetches BH register
uint8_t Bee8086::get_bh()
{
    return bx.gethi();
}

// Fetches BL register
uint8_t Bee8086::get_bl()
{
    return bx.getlo();
}

// Fetches BX register
uint16_t Bee8086::get_bx()
{
    return bx.getreg();
}

// Fetches CS register
uint16_t Bee8086::get_cs()
{
    return cs;
}

// Fetches ES register
uint16_t Bee8086::get_es()
{
    return es;
}

// Fetches IP register
uint16_t Bee8086::get_ip()
{
    return ip;
}

// Executes a single instruction and returns its cycle count
int Bee8086::runinstruction()
{
    return executenextopcode(getimmByte());
}

// Converts a segment and an offset to a physical address
uint32_t Bee8086::convertSeg(uint16_t seg, uint16_t offs)
{
    // Check if interface is valid (i.e. not a null pointer)
    // before accessing it (this helps prevent a buffer overflow caused
    // by an erroneous null pointer)

    if (inter != NULL)
    {
	return inter->convertSeg(seg, offs);
    }
    else
    {
	// Return 0 if interface is invalid
	return 0x00;
    }
}

// Reads an 8-bit value from memory at address of "addr"
uint8_t Bee8086::readByte(uint32_t addr)
{
    // Check if interface is valid (i.e. not a null pointer)
    // before accessing it (this helps prevent a buffer overflow caused
    // by an erroneous null pointer)

    if (inter != NULL)
    {
	return inter->readByte(addr);
    }
    else
    {
	// Return 0 if interface is invalid
	return 0x00;
    }
}
// Reads an 8-bit value from memory at address of "seg:offs"
uint8_t Bee8086::readByte(uint16_t seg, uint16_t offs)
{
    return readByte(convertSeg(seg, offs));
}

// Writes an 8-bit value "val" to memory at address of "addr"
void Bee8086::writeByte(uint32_t addr, uint8_t val)
{
    // Check if interface is valid (i.e. not a null pointer)
    // before accessing it (this helps prevent a buffer overflow caused
    // by an erroneous null pointer)

    if (inter != NULL)
    {
	inter->writeByte(addr, val);
    }
}

// Writes an 8-bit value "val" to memory at address of "seg:offs"
void Bee8086::writeByte(uint16_t seg, uint16_t offs, uint8_t val)
{
    writeByte(convertSeg(seg, offs), val);
}

// Reads a 16-bit value from memory at address of "addr"
uint16_t Bee8086::readWord(uint32_t addr)
{
    // Check if interface is valid (i.e. not a null pointer)
    // before accessing it (this helps prevent a buffer overflow caused
    // by an erroneous null pointer)

    // The Intel 8086 is a little-endian system,
    // so the 16-bit value is constructed as follows:
    // val_16 = (mem[addr + 1] << 8) | mem[addr])

    uint8_t lo_byte = readByte(addr);
    uint8_t hi_byte = readByte(addr + 1);
    return ((hi_byte << 8) | lo_byte);
}

// Reads a 16-bit value from memory at address of "seg:offs"
uint16_t Bee8086::readWord(uint16_t seg, uint16_t offs)
{
    return readWord(convertSeg(seg, offs));
}

// Writes a 16-bit value "val" to memory at address of "addr"
void Bee8086::writeWord(uint32_t addr, uint16_t val)
{
    // Check if interface is valid (i.e. not a null pointer)
    // before accessing it (this helps prevent a buffer overflow caused
    // by an erroneous null pointer)

    // The Intel 8086 is a little-endian system,
    // so the 16-bit value is written as follows:
    // mem[addr] = low_byte(val)
    // mem[addr + 1] = high_byte(val)

    writeByte(addr, (val & 0xFF));
    writeByte((addr + 1), (val >> 8));
}

// Writes a 16-bit value "val" to memory at address of "seg:offs"
void Bee8086::writeWord(uint16_t seg, uint16_t offs, uint16_t val)
{
    writeWord(convertSeg(seg, offs), val);
}

// Reads a byte from an I/O device at port of "port"
uint8_t Bee8086::portIn(uint16_t port)
{
    // Check if interface is valid (i.e. not a null pointer)
    // before accessing it (this helps prevent a buffer overflow caused
    // by an erroneous null pointer)

    if (inter != NULL)
    {
	return inter->portIn(port);
    }
    else
    {
	// Return 0 if interface is invalid
	return 0x00;
    }
}

// Reads a word from an I/O device at port of "port"
uint16_t Bee8086::portIn16(uint16_t port)
{
    // The Intel 8086 is a little-endian system,
    // so the 16-bit value is constructed as follows:
    // val_16 = (port[addr + 1] << 8) | port[addr])
    uint8_t lo_byte = portIn(port);
    uint8_t hi_byte = portIn((port + 1));
    return ((hi_byte << 8) | lo_byte);
}

// Writes a byte of "val" to an I/O device at port of "port"
void Bee8086::portOut(uint16_t port, uint8_t val)
{
    // Check if interface is valid (i.e. not a null pointer)
    // before accessing it (this helps prevent a buffer overflow caused
    // by an erroneous null pointer)

    if (inter != NULL)
    {
	inter->portOut(port, val);
    }
}

// Writes a byte of "val" to an I/O device at port of "port"
void Bee8086::portOut16(uint16_t port, uint16_t val)
{
    // The Intel 8086 is a little-endian system,
    // so the 16-bit value is written as follows:
    // port[num] = low_byte(val)
    // port[num + 1] = high_byte(val)
    portOut(port, (val & 0xFF));
    portOut((port + 1), (val >> 8));
}

// Fetches subsequent byte from memory
uint8_t Bee8086::getimmByte()
{
    // Fetch the byte located at the address of the program counter...
    uint8_t value = readByte(convertSeg(cs, ip));

    // ...increment the program counter...
    ip += 1;

    // ...and then return the fetched value
    return value;
}

// Fetches subsequent word from memory
uint16_t Bee8086::getimmWord()
{
    // Fetch the 16-bit word located at the address of the program counter...
    uint16_t value = readWord(convertSeg(cs, ip));

    // ...increment the program counter by 2 (once for each fetched byte)...
    ip += 2;

    // ...and then return the fetched value
    return value;
}

bool Bee8086::isInterruptOverride(uint8_t int_num)
{
    if (inter == NULL)
    {
	return false;
    }

    return inter->isInterruptOverride(int_num);
}

void Bee8086::interruptOverride(uint8_t int_num)
{
    if (inter != NULL)
    {
	inter->interruptOverride(*this, int_num);
    }
}

// TODO: Improve accuracy of dissasembly output and opcode size
size_t Bee8086::disassembleinstr(ostream &stream, size_t pc)
{
    static uint32_t suppress = 0xFFFFFFFF;

    if (pc == suppress)
    {
	return 0;
    }

    size_t prev_pc = pc;

    uint8_t opcode = readByte(pc++);

    string repeat = "";
    string prefix = "";

    for (size_t i = 0; i < 7; i++)
    {
	if (opcode == 0x26)
	{
	    prefix = "es";
	    opcode = readByte(pc);
	    suppress = pc;
	    continue;
	}

	if (opcode == 0x2E)
	{
	    prefix = "cs";
	    opcode = readByte(pc);
	    suppress = pc;
	    continue;
	}

	if (opcode == 0x36)
	{
	    prefix = "ss";
	    opcode = readByte(pc);
	    suppress = pc;
	    continue;
	}

	if (opcode == 0x3E)
	{
	    prefix = "ds";
	    opcode = readByte(pc);
	    suppress = pc;
	    continue;
	}

	if (opcode == 0xF3)
	{
	    repeat = "rep";
	    opcode = readByte(pc);
	    suppress = pc;
	    continue;
	}

	break;
    }
    

    switch (opcode)
    {
	case 0x00: stream << "add reg8/mem8, reg8"; break;
	case 0x01: stream << "add reg16/mem16, reg16"; break;
	case 0x06: stream << "push es"; break;
	case 0x07: stream << "pop es"; break;
	case 0x0E: stream << "push cs"; break;
	case 0x0F: stream << "pop cs"; break;
	case 0x16: stream << "push ss"; break;
	case 0x17: stream << "pop ss"; break;
	case 0x1E: stream << "push ds"; break;
	case 0x1F: stream << "pop ds"; break;
	case 0x24:
	{
	    uint16_t imm_val = readByte(pc++);
	    stream << "and al, #$" << hex << int(imm_val);
	}
	break;
	case 0x26: stream << "es:"; break;
	case 0x2E: stream << "cs:"; break;
	case 0x36: stream << "ss:"; break;
	case 0x3E: stream << "ds:"; break;
	case 0x40: stream << "inc ax"; break;
	case 0x41: stream << "inc cx"; break;
	case 0x42: stream << "inc dx"; break;
	case 0x43: stream << "inc bx"; break;
	case 0x48: stream << "dec ax"; break;
	case 0x49: stream << "dec cx"; break;
	case 0x4A: stream << "dec dx"; break;
	case 0x4B: stream << "dec bx"; break;
	case 0x50: stream << "push ax"; break;
	case 0x51: stream << "push cx"; break;
	case 0x52: stream << "push dx"; break;
	case 0x53: stream << "push bx"; break;
	case 0x55: stream << "push bp"; break;
	case 0x58: stream << "pop ax"; break;
	case 0x59: stream << "pop cx"; break;
	case 0x5A: stream << "pop dx"; break;
	case 0x5B: stream << "pop bx"; break;
	case 0x5D: stream << "pop bp"; break;
	case 0x5E: stream << "pop si"; break;
	case 0x70:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jo $" << hex << int(addr);
	}
	break;
	case 0x71:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jno $" << hex << int(addr);
	}
	break;
	case 0x72:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jb $" << hex << int(addr);
	}
	break;
	case 0x73:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jnb $" << hex << int(addr);
	}
	break;
	case 0x74:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jz $" << hex << int(addr);
	}
	break;
	case 0x75:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jnz $" << hex << int(addr);
	}
	break;
	case 0x76:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jbe $" << hex << int(addr);
	}
	break;
	case 0x77:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "ja $" << hex << int(addr);
	}
	break;
	case 0x78:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "js $" << hex << int(addr);
	}
	break;
	case 0x79:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jns $" << hex << int(addr);
	}
	break;
	case 0x7A:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jpe $" << hex << int(addr);
	}
	break;
	case 0x7B:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jpo $" << hex << int(addr);
	}
	break;
	case 0x7C:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jl $" << hex << int(addr);
	}
	break;
	case 0x7D:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jge $" << hex << int(addr);
	}
	break;
	case 0x7E:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jle $" << hex << int(addr);
	}
	break;
	case 0x7F:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jg $" << hex << int(addr);
	}
	break;
	case 0x80:
	{
	    stream << "grp1 mem8, imm8";
	}
	break;
	case 0x84:
	{
	    dasmModRM(pc);
	    stream << "test reg8/mem8, reg8";
	    pc += 1;
	}
	break;
	case 0x88:
	{
	    stream << "mov reg8/mem8, reg8";
	    pc += 1;
	}
	break;
	case 0x89:
	{
	    stream << "mov reg16/mem16, reg16";
	    pc += 1;
	}
	break;
	case 0x8A:
	{
	    stream << "mov reg8, reg8/mem8";
	    pc += 1;
	}
	break;
	case 0x8B:
	{
	    stream << "mov reg16, reg16/mem16";
	    pc += 1;
	}
	break;
	case 0x8C:
	{
	    dasmModRM(pc);
	    stream << "mov " << dasm_mod_rm.dasm_str << ", " << dasmSeg(dasm_mod_rm.reg);
	    pc += 1;
	}
	break;
	case 0x8E:
	{
	    dasmModRM(pc);
	    stream << "mov " << dasmSeg(dasm_mod_rm.reg) << ", " << dasm_mod_rm.dasm_str;
	    pc += 1;
	}
	break;
	case 0x9D: stream << "popf"; break;
	case 0x9E: stream << "sahf"; break;
	case 0x9F: stream << "lahf"; break;
	case 0xA2:
	{
	    stream << "mov mem8, al";
	}
	break;
	case 0xA4:
	{
	    if (repeat != "")
	    {
		stream << repeat << " ";
	    }

	    stream << "movsb";
	}
	break;
	case 0xAB:
	{
	    if (repeat != "")
	    {
		stream << repeat << " ";
	    }

	    stream << "stosw";
	}
	break;
	case 0xB0:
	{
	    uint16_t imm_val = readByte(pc);
	    stream << "mov al, #$" << hex << int(imm_val);
	    pc += 1;
	}
	break;
	case 0xB1:
	{
	    uint16_t imm_val = readByte(pc);
	    stream << "mov cl, #$" << hex << int(imm_val);
	    pc += 1;
	}
	break;
	case 0xB2:
	{
	    uint16_t imm_val = readByte(pc);
	    stream << "mov dl, #$" << hex << int(imm_val);
	    pc += 1;
	}
	break;
	case 0xB3:
	{
	    uint16_t imm_val = readByte(pc);
	    stream << "mov bl, #$" << hex << int(imm_val);
	    pc += 1;
	}
	break;
	case 0xB4:
	{
	    uint16_t imm_val = readByte(pc);
	    stream << "mov ah, #$" << hex << int(imm_val);
	    pc += 1;
	}
	break;
	case 0xB5:
	{
	    uint16_t imm_val = readByte(pc);
	    stream << "mov ch, #$" << hex << int(imm_val);
	    pc += 1;
	}
	break;
	case 0xB6:
	{
	    uint16_t imm_val = readByte(pc);
	    stream << "mov dh, #$" << hex << int(imm_val);
	    pc += 1;
	}
	break;
	case 0xB7:
	{
	    uint16_t imm_val = readByte(pc);
	    stream << "mov bh, #$" << hex << int(imm_val);
	    pc += 1;
	}
	break;
	case 0xB8:
	{
	    uint16_t imm_val = readWord(pc);
	    stream << "mov ax, #$" << hex << int(imm_val);
	    pc += 2;
	}
	break;
	case 0xB9:
	{
	    uint16_t imm_val = readWord(pc);
	    stream << "mov cx, #$" << hex << int(imm_val);
	    pc += 2;
	}
	break;
	case 0xBA:
	{
	    uint16_t imm_val = readWord(pc);
	    stream << "mov dx, #$" << hex << int(imm_val);
	    pc += 2;
	}
	break;
	case 0xBB:
	{
	    uint16_t imm_val = readWord(pc);
	    stream << "mov bx, #$" << hex << int(imm_val);
	    pc += 2;
	}
	break;
	case 0xBC:
	{
	    uint16_t imm_val = readWord(pc);
	    stream << "mov sp, #$" << hex << int(imm_val);
	    pc += 2;
	}
	break;
	case 0xBE:
	{
	    uint16_t imm_val = readWord(pc);
	    stream << "mov si, #$" << hex << int(imm_val);
	    pc += 2;
	}
	break;
	case 0xBF:
	{
	    uint16_t imm_val = readWord(pc);
	    stream << "mov di, #$" << hex << int(imm_val);
	    pc += 2;
	}
	break;
	case 0xC3: stream << "ret"; break;
	case 0xCD:
	{
	    uint8_t int_num = readByte(pc++);
	    stream << "int " << hex << int(int_num);
	}
	break;
	case 0xCF: stream << "iret"; break;
	case 0xD0:
	{
	    stream << "grp2 mem8, 1";
	}
	break;
	case 0xD2:
	{
	    stream << "grp2 mem8, CL";
	}
	break;
	case 0xE2: stream << "loop"; break;
	case 0xE4:
	{
	    uint8_t imm = readByte(pc++);
	    stream << "in al, $" << hex << int(imm);
	}
	break;
	case 0xE6:
	{
	    uint8_t imm = readByte(pc++);
	    stream << "out $" << hex << int(imm) << ", al";
	}
	break;
	case 0xE8:
	{
	    int16_t offs = readWord(pc);
	    pc += 2;
	    uint32_t addr = (pc + offs);

	    stream << "call $" << hex << int(addr);
	}
	break;
	case 0xEA:
	{
	    uint16_t ip_val = readWord(pc);
	    pc += 2;
	    uint16_t cs_val = readWord(pc);
	    pc += 2;

	    stream << "jmp " << hex << int(cs_val) << ":" << hex << int(ip_val);
	}
	break;
	case 0xEB:
	{
	    int8_t imm = readByte(pc++);
	    uint32_t addr = (pc + imm);
	    stream << "jmp $" << hex << int(addr);
	}
	break;
	case 0xEF: stream << "out dx, ax"; break;
	case 0xF3: stream << "rep"; break;
	case 0xFA: stream << "cli"; break;
	case 0xFB: stream << "sti"; break;
	case 0xFC: stream << "cld"; break;
	case 0xFF:
	{
	    stream << "grp5 mem";
	}
	break;
	default: stream << "unk " << hex << int(opcode); break;
    }

    return (pc - prev_pc);
}

// Emulates the individual Intel 8086 instructions
int Bee8086::executenextopcode(uint8_t opcode)
{
    int temp = 0;

    switch (opcode)
    {
	case 0x00: temp = addMemReg(); break; // ADD reg8/mem8, reg8
	case 0x01: temp = addMemReg16(); break; // ADD reg16/mem16, reg16
	case 0x06: temp = pushSeg(0); break; // PUSH ES
	case 0x07: temp = popSeg(0); break; // POP ES
	case 0x0E: temp = pushSeg(1); break; // PUSH CS
	case 0x0F: temp = popSeg(1); break; // POP CS
	case 0x16: temp = pushSeg(2); break; // PUSH SS
	case 0x17: temp = popSeg(2); break; // POP SS
	case 0x1E: temp = pushSeg(3); break; // PUSH DS
	case 0x1F: temp = popSeg(3); break; // POP DS
	case 0x24: ax.setlo(and_byte(ax.getlo(), getimmByte())); temp = 4; break; // AND AL, imm8
	case 0x26: temp = segmentOverride(Segment::Extra); break; // ES:
	case 0x2E: temp = segmentOverride(Segment::Code); break; // CS:
	case 0x36: temp = segmentOverride(Segment::Stack); break; // SS:
	case 0x3E: temp = segmentOverride(Segment::Data); break; // DS:
	case 0x40: ax.setreg(inc_word(ax.getreg())); temp = 3; break; // INC AX
	case 0x41: cx.setreg(inc_word(cx.getreg())); temp = 3; break; // INC CX
	case 0x42: dx.setreg(inc_word(dx.getreg())); temp = 3; break; // INC DX
	case 0x43: bx.setreg(inc_word(bx.getreg())); temp = 3; break; // INC BX
	case 0x44: sp = inc_word(sp); temp = 3; break; // INC SP
	case 0x45: bp = inc_word(bp); temp = 3; break; // INC BP
	case 0x46: si = inc_word(si); temp = 3; break; // INC SI
	case 0x47: di = inc_word(di); temp = 3; break; // INC DI
	case 0x48: ax.setreg(dec_word(ax.getreg())); temp = 3; break; // DEC AX
	case 0x49: cx.setreg(dec_word(cx.getreg())); temp = 3; break; // DEC CX
	case 0x4A: dx.setreg(dec_word(dx.getreg())); temp = 3; break; // DEC DX
	case 0x4B: bx.setreg(dec_word(bx.getreg())); temp = 3; break; // DEC BX
	case 0x4C: sp = dec_word(sp); temp = 3; break; // DEC SP
	case 0x4D: bp = dec_word(bp); temp = 3; break; // DEC BP
	case 0x4E: si = dec_word(si); temp = 3; break; // DEC SI
	case 0x4F: di = dec_word(di); temp = 3; break; // DEC DI
	case 0x50: temp = pushReg(ax.getreg()); break; // PUSH AX
	case 0x51: temp = pushReg(cx.getreg()); break; // PUSH CX
	case 0x52: temp = pushReg(dx.getreg()); break; // PUSH DX
	case 0x53: temp = pushReg(bx.getreg()); break; // PUSH BX
	case 0x55: temp = pushReg(bp); break; // PUSH BP
	case 0x58: temp = popReg(ax); break; // POP AX
	case 0x59: temp = popReg(cx); break; // POP CX
	case 0x5A: temp = popReg(dx); break; // POP DX
	case 0x5B: temp = popReg(bx); break; // POP BX
	case 0x5D: temp = popReg(bp); break; // POP BP
	case 0x5E: temp = popReg(si); break; // POP SI
	case 0x70: temp = jumpShort(is_overflow()); break; // JO imm8
	case 0x71: temp = jumpShort(!is_overflow()); break; // JNO imm8
	case 0x72: temp = jumpShort(is_carry()); break; // JB imm8
	case 0x73: temp = jumpShort(!is_carry()); break; // JNB imm8
	case 0x74: temp = jumpShort(is_zero()); break; // JZ imm8
	case 0x75: temp = jumpShort(!is_zero()); break; // JNZ imm8
	case 0x76: temp = jumpShort(is_below_or_equal()); break; // JBE imm8
	case 0x77: temp = jumpShort(!is_below_or_equal()); break; // JA imm8
	case 0x78: temp = jumpShort(is_sign()); break; // JS imm8
	case 0x79: temp = jumpShort(!is_sign()); break; // JNS imm8
	case 0x7A: temp = jumpShort(is_parity()); break; // JPE imm8
	case 0x7B: temp = jumpShort(!is_parity()); break; // JPO imm8
	case 0x7C: temp = jumpShort(is_less_than()); break; // JL imm8
	case 0x7D: temp = jumpShort(!is_less_than()); break; // JGE imm8
	case 0x7E: temp = jumpShort(is_less_than_or_equal()); break; // JLE imm8
	case 0x7F: temp = jumpShort(!is_less_than_or_equal()); break; // JG imm8
	case 0x80: temp = group1MemImm(); break; // GRP1 mem8, imm8
	case 0x84: temp = testMemReg(); break; // TEST reg8/mem8, reg8
	case 0x88: temp = moveMemReg(); break; // MOV reg8/mem8, reg8
	case 0x89: temp = moveMemReg16(); break; // MOV reg16/mem16, reg16
	case 0x8A: temp = moveRegMem(); break; // MOV reg8, reg8/mem8
	case 0x8B: temp = moveRegMem16(); break; // MOV reg16, reg16/mem16
	case 0x8C: temp = moveMemSeg16(); break; // MOV reg16/mem16, seg16
	case 0x8E: temp = moveSegMem16(); break; // MOV seg16, reg16/mem16
	case 0x9D: temp = popFlags(); break; // POPF
	case 0x9E: temp = storeFlagsAcc(); break; // SAHF
	case 0x9F: temp = loadAccFlags(); break; // LAHF
	case 0xA0: temp = moveAccMem(); break; // MOV AL, mem8
	case 0xA1: temp = moveAccMem16(); break; // MOV AL, mem16
	case 0xA2: temp = moveMemAcc(); break; // MOV mem8, AL
	case 0xA3: temp = moveMemAcc16(); break; // MOV mem16, AL
	case 0xA4: temp = moveStringByte(); break; // MOVSB
	case 0xAB: temp = storeStringWord(); break; // STOSW
	case 0xB0: ax.setlo(getimmByte()); temp = 4; break; // MOV AL, imm8
	case 0xB1: cx.setlo(getimmByte()); temp = 4; break; // MOV CL, imm8
	case 0xB2: dx.setlo(getimmByte()); temp = 4; break; // MOV DL, imm8
	case 0xB3: bx.setlo(getimmByte()); temp = 4; break; // MOV BL, imm8
	case 0xB4: ax.sethi(getimmByte()); temp = 4; break; // MOV AH, imm8
	case 0xB5: cx.sethi(getimmByte()); temp = 4; break; // MOV CH, imm8
	case 0xB6: dx.sethi(getimmByte()); temp = 4; break; // MOV DH, imm8
	case 0xB7: bx.sethi(getimmByte()); temp = 4; break; // MOV BH, imm8
	case 0xB8: ax.setreg(getimmWord()); temp = 4; break; // MOV AX, imm16
	case 0xB9: cx.setreg(getimmWord()); temp = 4; break; // MOV CX, imm16
	case 0xBA: dx.setreg(getimmWord()); temp = 4; break; // MOV CX, imm16
	case 0xBB: bx.setreg(getimmWord()); temp = 4; break; // MOV BX, imm16
	case 0xBC: sp = getimmWord(); temp = 4; break; // MOV SP, imm16
	case 0xBE: si = getimmWord(); temp = 4; break; // MOV SI, imm16
	case 0xBF: di = getimmWord(); temp = 4; break; // MOV DI, imm16
	case 0xC3: temp = retNear(); break; // RET near
	case 0xCD: interruptCall(getimmByte()); temp = 51; break; // INT imm8
	case 0xCF: temp = intRet(); break; // IRET
	case 0xD0: temp = group2Mem(); break; // GRP1 mem8, 1
	case 0xD2: temp = group2Mem(false, cx.getlo()); break; // GRP1 mem8, CL
	case 0xE2: temp = loopCond(); break; // LOOP imm8
	case 0xE4: ax.setlo(portIn(getimmByte())); temp = 10; break; // IN AL, imm8
	case 0xE6: portOut(getimmByte(), ax.getlo()); temp = 10; break; // OUT imm8, AL
	case 0xE8: temp = callNear(); break; // CALL near-label
	case 0xEA: temp = jumpFar(); break; // JMP far-label
	case 0xEB: jumpShort(); temp = 15; break; // JMP short-label
	case 0xEE: portOut(dx.getreg(), ax.getlo()); temp = 10; break; // OUT DX, AL
	case 0xEF: portOut16(dx.getreg(), ax.getreg()); temp = 8; break; // OUT DX, AX
	case 0xF3: is_rep = true; temp = 2; break; // REP
	case 0xFA: set_irq(false); temp = 2; break; // CLI
	case 0xFB: set_irq(true); temp = 2; break; // STI
	case 0xFC: set_direction(false); temp = 2; break; // CLD
	case 0xFE: temp = group4Mem(); break; // GRP4 mem8
	case 0xFF: temp = group5Mem(); break; // GRP5 mem16
	default: unrecognizedopcode(opcode); break;
    }

    return temp;
}

// This function is called when the emulated Intel 8086 encounters
// a CPU instruction it doesn't recgonize
void Bee8086::unrecognizedopcode(uint8_t opcode)
{
    cout << "Fatal: Unrecognized opcode of " << hex << (int)(opcode) << endl;
    exit(1);
}