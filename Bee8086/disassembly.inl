auto dasmModRM(size_t &pc) -> void
{
    uint8_t byte = readByte(pc);
    dasm_mod_rm.mod = ((byte >> 6) & 0x3);
    dasm_mod_rm.reg = ((byte >> 3) & 0x7);
    dasm_mod_rm.mem = (byte & 0x7);

    stringstream dasm_str;

    if (dasm_mod_rm.mod == 3)
    {
	dasm_str << "reg16";
    }
    else
    {
	dasm_str << "mem16";
    }

    dasm_mod_rm.dasm_str = dasm_str.str();
};

auto dasmSeg(int reg) -> string
{
    stringstream ss;
    reg &= 3;

    switch (reg)
    {
	case 0: ss << "es"; break;
	case 1: ss << "cs"; break;
	case 2: ss << "ss"; break;
	case 3: ss << "ds"; break;
	default: ss << "unk"; break;
    }

    return ss.str();
};