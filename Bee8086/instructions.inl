auto add_internal_byte(uint8_t source, uint8_t operand) -> uint8_t
{
    uint8_t result = (source + operand);
    set_sign(testbit(result, 7));
    set_zero(result == 0);
    set_carry((source + operand) > 0xFF);
    return result;
}

auto add_internal_word(uint16_t source, uint16_t operand) -> uint16_t
{
    uint16_t result = (source + operand);
    set_sign(testbit(result, 15));
    set_zero(result == 0);
    set_carry((source + operand) > 0xFFFF);
    return result;
}

auto sub_internal_byte(uint8_t source, uint8_t operand) -> uint8_t
{
    uint8_t result = (source - operand);
    set_sign(testbit(result, 7));
    set_zero(result == 0);
    set_carry(operand > source);
    return result;
}

auto and_internal_byte(uint8_t source, uint8_t operand) -> uint8_t
{
    uint8_t result = (source & operand);
    set_sign(testbit(result, 7));
    set_zero(result == 0);
    set_carry(false);
    return result;
}

auto or_internal_byte(uint8_t source, uint8_t operand) -> uint8_t
{
    uint8_t result = (source & operand);
    set_sign(testbit(result, 7));
    set_zero(result == 0);
    set_carry(false);
    return result;
}

auto shl_internal_byte(uint8_t source, uint8_t shift_amount) -> uint8_t
{
    shift_amount &= 0x1F;
    set_carry(testbit(source, 7));
    uint8_t result = (source << shift_amount);
    set_sign(testbit(result, 7));
    set_zero(result == 0);
    return result;
}

auto shr_internal_byte(uint8_t source, uint8_t shift_amount) -> uint8_t
{
    shift_amount &= 0x1F;
    set_carry(testbit(source, 0));
    uint8_t result = (source >> shift_amount);
    set_sign(testbit(result, 7));
    set_zero(result == 0);
    return result;
}

auto inc_internal_byte(uint8_t source) -> uint8_t
{
    uint8_t result = (source + 1);
    set_sign(testbit(result, 7));
    set_zero(result == 0);
    return result;
}

auto dec_internal_byte(uint8_t source) -> uint8_t
{
    uint8_t result = (source - 1);
    set_sign(testbit(result, 7));
    set_zero(result == 0);
    return result;
}

auto inc_internal_word(uint16_t source) -> uint16_t
{
    uint16_t result = (source + 1);
    set_sign(testbit(result, 15));
    set_zero(result == 0);
    return result;
}

auto dec_internal_word(uint16_t source) -> uint16_t
{
    uint16_t result = (source - 1);
    set_sign(testbit(result, 15));
    set_zero(result == 0);
    return result;
}

auto add_byte(uint8_t source, uint8_t operand) -> uint8_t
{
    return add_internal_byte(source, operand);
}

auto add_word(uint16_t source, uint16_t operand) -> uint16_t
{
    return add_internal_word(source, operand);
}

auto adc_byte(uint8_t source, uint8_t operand) -> uint8_t
{
    return add_internal_byte(source, (operand + is_carry()));
}

auto cmp_byte(uint8_t source, uint8_t operand) -> void
{
    sub_internal_byte(source, operand);
}

auto test_byte(uint8_t source, uint8_t operand) -> void
{
    and_internal_byte(source, operand);
}

auto and_byte(uint8_t source, uint8_t operand) -> uint8_t
{
    return and_internal_byte(source, operand);
}

auto or_byte(uint8_t source, uint8_t operand) -> uint8_t
{
    return or_internal_byte(source, operand);
}

auto shl_byte(uint8_t source, uint8_t shift_amount) -> uint8_t
{
    return shl_internal_byte(source, shift_amount);
}

auto shr_byte(uint8_t source, uint8_t shift_amount) -> uint8_t
{
    return shr_internal_byte(source, shift_amount);
}

auto inc_byte(uint8_t source) -> uint8_t
{
    return inc_internal_byte(source);
}

auto dec_byte(uint8_t source) -> uint8_t
{
    return dec_internal_byte(source);
}

auto inc_word(uint16_t source) -> uint16_t
{
    return inc_internal_word(source);
}

auto dec_word(uint16_t source) -> uint16_t
{
    return dec_internal_word(source);
}

auto jumpShort(bool cond = true) -> int
{
    int8_t offs = getimmByte();

    if (cond == true) 
    {
	ip += offs;
	return 16;
    }

    return 4;
}

auto callNear() -> int
{
    int16_t offs = getimmWord();
    pushReg(ip);
    ip += offs;
    return 19;
}

auto retNear() -> int
{
    popReg(ip);
    return 16;
}

auto jumpFar() -> int
{
    uint16_t ip_val = getimmWord();
    uint16_t cs_val = getimmWord();

    ip = ip_val;
    cs = cs_val;
    return 15;
}

auto loopCond(bool cond = true) -> int
{
    int8_t offs = getimmByte();

    cx.setreg(cx.getreg() - 1);

    bool branch = ((cx.getreg() != 0) && cond);

    int cycles = 0;

    if (!branch)
    {
	cycles = 4;
    }
    else
    {
	ip += offs;
	cycles = 8;
    }

    return cycles;
}

auto storeFlagsAcc() -> int
{
    status_reg = ((status_reg & 0xFF00) | ax.gethi());
    return 4;
}

auto loadAccFlags() -> int
{
    ax.sethi((status_reg & 0xFF));
    return 4;
}

auto addMemReg() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    setMem(add_byte(getMem(), getReg()));

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += 3;
    }
    else
    {
	mod_cycles += 16;
    }

    return mod_cycles;
}

auto addMemReg16() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    setMem16(add_word(getMem16(), getReg16()));

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += 3;
    }
    else
    {
	mod_cycles += 16;
    }

    return mod_cycles;
}

auto moveMemSeg16() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    setMem16(getSeg(current_mod_rm.reg));

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += 2;
    }
    else
    {
	mod_cycles += 9;
    }

    return mod_cycles;
}

auto moveSegMem16() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    setSeg(current_mod_rm.reg, getMem16());

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += 2;
    }
    else
    {
	mod_cycles += 8;
    }

    return mod_cycles;
}

auto moveRegMem() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    setReg(getMem());

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += 2;
    }
    else
    {
	mod_cycles += 8;
    }

    return mod_cycles;
}

auto moveMemReg() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    setMem(getReg());

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += 2;
    }
    else
    {
	mod_cycles += 9;
    }

    return mod_cycles;
}

auto moveMemReg16() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    setMem16(getReg16());

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += 2;
    }
    else
    {
	mod_cycles += 9;
    }

    return mod_cycles;
}

auto moveRegMem16() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    setReg16(getMem16());

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += 2;
    }
    else
    {
	mod_cycles += 8;
    }

    return mod_cycles;
};

auto moveAccMem() -> int
{
    uint16_t addr = getimmWord();
    ax.setlo(readByte(getSegment(1), addr));
    return 10;
}

auto moveMemAcc() -> int
{
    uint16_t addr = getimmWord();
    writeByte(getSegment(1), addr, ax.getlo());
    return 10;
}

auto moveAccMem16() -> int
{
    uint16_t addr = getimmWord();
    ax.setreg(readWord(getSegment(1), addr));
    return 10;
}

auto moveMemAcc16() -> int
{
    uint16_t addr = getimmWord();
    writeWord(getSegment(1), addr, ax.getreg());
    return 10;
}

auto testMemReg() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    uint8_t mem_val = getMem();
    uint8_t reg_val = getReg();

    test_byte(mem_val, reg_val);

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += 3;
    }
    else
    {
	mod_cycles += 9;
    }

    return mod_cycles;
};

auto pushSeg(int index) -> int
{
    sp -= 2;
    writeWord(ss, sp, getSeg(index));
    return 10;
}

auto popSeg(int index) -> int
{
    uint16_t data = readWord(ss, sp);
    sp += 2;
    setSeg(index, data);
    return 8;
}

auto pushReg(uint16_t val) -> int
{
    sp -= 2;
    writeWord(ss, sp, val);
    return 11;
}

auto popReg(uint16_t &reg) -> int
{
    uint16_t data = readWord(ss, sp);
    sp += 2;
    reg = data;
    return 8;
}

auto popReg(Bee8086Register &reg) -> int
{
    uint16_t data = readWord(ss, sp);
    sp += 2;
    reg.setreg(data);
    return 8;
}

auto popFlags() -> int
{
    uint16_t data = readWord(ss, sp);
    sp += 2;
    status_reg = data;
    return 8;
}

// TODO: Determine correct timings for this instruction
auto moveStringByte() -> int
{
    if (!is_rep || cx.getreg() > 0)
    {
	uint8_t str_byte = readByte(getSegment(1), si);
	writeByte(es, di, str_byte);

	if (is_direction())
	{
	    si -= 1;
	    di -= 1;
	}
	else
	{
	    si += 1;
	    di += 1;
	}

	if (is_rep)
	{
	    cx.setreg(cx.getreg() - 1); // cx -= 1;

	    if (cx.getreg() == 0)
	    {
		is_rep = false;
	    }
	    else
	    {
		ip -= 1;
	    }
	}
    }

    return 18;
}

// TODO: Determine correct timings for this instruction
auto storeStringWord() -> int
{
    if (!is_rep || cx.getreg() > 0)
    {
	writeWord(es, di, ax.getreg());

	if (is_direction())
	{
	    si -= 2;
	    di -= 2;
	}
	else
	{
	    si += 2;
	    di += 2;
	}

	if (is_rep)
	{
	    cx.setreg(cx.getreg() - 1); // cx -= 1;

	    if (cx.getreg() == 0)
	    {
		is_rep = false;
	    }
	    else
	    {
		ip -= 1;
	    }
	}
    }

    return 11;
}

auto group1MemImm(bool sign = false) -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    uint8_t memory = getMem();

    uint16_t imm = 0;

    if (sign)
    {
	imm = int8_t(getimmByte());
    }
    else
    {
	imm = getimmByte();
    }

    int cycles_reg = 0;
    int cycles_mem = 0;

    switch (current_mod_rm.reg)
    {
	case 0:
	{
	    setMem(add_byte(memory, imm));
	    cycles_reg = 4;
	    cycles_mem = 17;
	}
	break;
	case 1:
	{
	    setMem(or_byte(memory, imm));
	    cycles_reg = 4;
	    cycles_mem = 17;
	}
	break;
	case 2:
	{
	    setMem(adc_byte(memory, imm));
	    cycles_reg = 4;
	    cycles_mem = 17;
	}
	break;
	case 4:
	{
	    setMem(and_byte(memory, imm));
	    cycles_reg = 4;
	    cycles_mem = 17;
	}
	break;
	case 7:
	{
	    cmp_byte(memory, imm);
	    cycles_reg = 4;
	    cycles_mem = 10;
	}
	break;
	default:
	{
	    cout << "Unrecognized group 1 register number of " << dec << int(current_mod_rm.reg) << endl;
	    exit(1);
	}
	break;
    }

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += cycles_reg;
    }
    else
    {
	mod_cycles += cycles_mem;
    }

    return mod_cycles;
}

auto group2Mem(bool is_constant = true, uint8_t val = 0) -> int
{
    uint8_t offs = (is_constant) ? 1 : val;

    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    uint8_t memory = getMem();

    int cycles_reg = (is_constant) ? 2 : 12;
    int cycles_mem = (is_constant) ? 12 : 24;

    switch (current_mod_rm.reg)
    {
	case 4: setMem(shl_byte(memory, offs)); break;
	case 5: setMem(shr_byte(memory, offs)); break;
	default:
	{
	    cout << "Unrecognized group 2 register number of " << dec << int(current_mod_rm.reg) << endl;
	    exit(1);
	}
	break;
    }

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += cycles_reg;
    }
    else
    {
	mod_cycles += cycles_mem;
    }

    return mod_cycles;
}

auto group4Mem() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    int cycles_reg = 0;
    int cycles_mem = 0;

    switch (current_mod_rm.reg)
    {
	case 0:
	{
	    setMem(inc_byte(getMem()));
	    cycles_reg = 3;
	    cycles_mem = 15;
	}
	break;
	case 1:
	{
	    setMem(dec_byte(getMem()));
	    cycles_reg = 3;
	    cycles_mem = 15;
	}
	break;
	default:
	{
	    cout << "Unrecognized group 4 register number of " << dec << int(current_mod_rm.reg) << endl;
	    exit(1);
	}
	break;
    }

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += cycles_reg;
    }
    else
    {
	mod_cycles += cycles_mem;
    }

    return mod_cycles;
}

auto group5Mem() -> int
{
    uint8_t mod_rm = getimmByte();
    int mod_cycles = decodeModRM(mod_rm);

    int cycles_reg = 0;
    int cycles_mem = 0;

    switch (current_mod_rm.reg)
    {
	case 0:
	{
	    setMem16(inc_word(getMem16()));
	    cycles_reg = 3;
	    cycles_mem = 15;
	}
	break;
	case 1:
	{
	    setMem16(dec_word(getMem16()));
	    cycles_reg = 3;
	    cycles_mem = 15;
	}
	break;
	case 4:
	{
	    ip = getMem16();
	    cycles_reg = 11;
	    cycles_mem = 18;
	}
	break;
	default:
	{
	    cout << "Unrecognized group 5 register number of " << dec << int(current_mod_rm.reg) << endl;
	    exit(1);
	}
	break;
    }

    if (current_mod_rm.mod == 3)
    {
	mod_cycles += cycles_reg;
    }
    else
    {
	mod_cycles += cycles_mem;
    }

    return mod_cycles;
}


auto interruptCall(uint8_t int_num) -> void
{
    if (isInterruptOverride(int_num))
    {
	interruptOverride(int_num);
	return;
    }

    pushReg(status_reg);
    pushReg(cs);
    pushReg(ip);

    uint32_t int_addr = (int_num * 4);
    ip = readWord(int_addr);
    cs = readWord(int_addr + 2);
}

auto intRet() -> int
{
    popReg(ip);
    popReg(cs);
    popReg(status_reg);
    return 24;
}

auto getReg() -> uint8_t
{
    uint8_t data = 0;
    int reg = current_mod_rm.reg;
    reg &= 7;

    switch (reg)
    {
	case 0: data = ax.getlo(); break;
	case 1: data = cx.getlo(); break;
	case 2: data = dx.getlo(); break;
	case 3: data = bx.getlo(); break;
	case 4: data = ax.gethi(); break;
	case 5: data = cx.gethi(); break;
	case 6: data = dx.gethi(); break;
	case 7: data = bx.gethi(); break;
    }

    return data;
};

auto setReg(uint8_t data) -> void
{
    int reg = current_mod_rm.reg;
    reg &= 7;

    switch (reg)
    {
	case 0: ax.setlo(data); break;
	case 1: cx.setlo(data); break;
	case 2: dx.setlo(data); break;
	case 3: bx.setlo(data); break;
	case 4: ax.sethi(data); break;
	case 5: cx.sethi(data); break;
	case 6: dx.sethi(data); break;
	case 7: bx.sethi(data); break;
	default: // This shouldn't happen
	{
	    cout << "Unrecognized register number of " << dec << int(reg) << endl;
	    exit(1);
	}
 	break;
    }
}

auto getReg16() -> uint16_t
{
    uint16_t data = 0;
    int reg = current_mod_rm.reg;
    reg &= 7;

    switch (reg)
    {
	case 0: data = ax.getreg(); break;
	case 1: data = cx.getreg(); break;
	case 2: data = dx.getreg(); break;
	case 3: data = bx.getreg(); break;
	case 4: data = sp; break;
	case 5: data = bp; break;
	case 6: data = si; break;
	case 7: data = di; break;
	default: // This shouldn't happen
	{
	    cout << "Unrecognized register number of " << dec << int(reg) << endl;
	    exit(1);
	}
 	break;
    }

    return data;
}

auto setReg16(uint16_t data) -> void
{
    int reg = current_mod_rm.reg;
    reg &= 7;

    switch (reg)
    {
	case 0: ax.setreg(data); break;
	case 1: cx.setreg(data); break;
	case 2: dx.setreg(data); break;
	case 3: bx.setreg(data); break;
	case 4: sp = data; break;
	case 5: bp = data; break;
	case 6: si = data; break;
	case 7: di = data; break;
	default: // This shouldn't happen
	{
	    cout << "Unrecognized register number of " << dec << int(reg) << endl;
	    exit(1);
	}
 	break;
    }
}

auto getMem(uint32_t offs = 0) -> uint8_t
{
    uint8_t temp = 0;

    if (current_mod_rm.mod != 3)
    {
	uint16_t seg = current_mod_rm.segment;
	uint16_t addr = current_mod_rm.addr;
	temp = readByte(seg, (addr + offs));
    }
    else
    {
	switch (current_mod_rm.mem)
	{
	    case 0: temp = ax.getlo(); break;
	    case 1: temp = cx.getlo(); break;
	    case 2: temp = dx.getlo(); break;
	    case 3: temp = bx.getlo(); break;
	    case 4: temp = ax.gethi(); break;
	    case 5: temp = cx.gethi(); break;
	    case 6: temp = dx.gethi(); break;
	    case 7: temp = bx.gethi(); break;
	    default: // This shouldn't happen
	    {
		cout << "Unrecognized register number of " << dec << int(current_mod_rm.mem) << endl;
		exit(1);
	    }
 	    break;
	}
    }

    return temp;
}

auto setMem(uint8_t data) -> void
{
    if (current_mod_rm.mod != 3)
    {
	uint16_t seg = current_mod_rm.segment;
	uint16_t addr = current_mod_rm.addr;
	writeByte(seg, addr, data);
    }
    else
    {
	switch (current_mod_rm.mem)
	{
	    case 0: ax.setlo(data); break;
	    case 1: cx.setlo(data); break;
	    case 2: dx.setlo(data); break;
	    case 3: bx.setlo(data); break;
	    case 4: ax.sethi(data); break;
	    case 5: cx.sethi(data); break;
	    case 6: dx.sethi(data); break;
	    case 7: bx.sethi(data); break;
	    default: // This shouldn't happen
	    {
		cout << "Unrecognized register number of " << dec << int(current_mod_rm.mem) << endl;
		exit(1);
	    }
 	    break;
	}
    }
}

auto getMem16(uint32_t offs = 0) -> uint16_t
{
    uint16_t temp = 0;

    if (current_mod_rm.mod != 3)
    {
	uint16_t seg = current_mod_rm.segment;
	uint16_t addr = current_mod_rm.addr;
	temp = readWord(seg, (addr + offs));
    }
    else
    {
	switch (current_mod_rm.mem)
	{
	    case 0: temp = ax.getreg(); break;
	    case 1: temp = cx.getreg(); break;
	    case 2: temp = dx.getreg(); break;
	    case 3: temp = bx.getreg(); break;
	    case 4: temp = sp; break;
	    case 5: temp = bp; break;
	    case 6: temp = si; break;
	    case 7: temp = di; break;
	    default: // This shouldn't happen
	    {
		cout << "Unrecognized register number of " << dec << int(current_mod_rm.mem) << endl;
		exit(1);
	    }
 	    break;
	}
    }

    return temp;
}

auto setMem16(uint16_t data) -> void
{
    if (current_mod_rm.mod != 3)
    {
	uint16_t seg = current_mod_rm.segment;
	uint16_t addr = current_mod_rm.addr;
	writeWord(seg, addr, data);
    }
    else
    {
	switch (current_mod_rm.mem)
	{
	    case 0: ax.setreg(data); break;
	    case 1: cx.setreg(data); break;
	    case 2: dx.setreg(data); break;
	    case 3: bx.setreg(data); break;
	    case 4: sp = data; break;
	    case 5: bp = data; break;
	    case 6: si = data; break;
	    case 7: di = data; break;
	    default: // This shouldn't happen
	    {
		cout << "Unrecognized register number of " << dec << int(current_mod_rm.mem) << endl;
		exit(1);
	    }
 	    break;
	}
    }
}

auto segmentOverride(Segment seg) -> int
{
    mem_segment = seg;
    is_segment_override = true;
    return 2;
}

// Fetches appropriate memory segment register
auto getSegment(int index) -> uint16_t
{
    // Select the segment register corresponding to "memsegment",
    // or the one corresponding to "index" if "memsegment" is
    // set to its default value

    // Indexes to segment registers:
    // 0 - Code segment register
    // 1 - Data segment register
    // 2 - Stack segment register
    // 3 - Extra segment register

    uint16_t temp = 0;

    switch (mem_segment)
    {
	case Segment::Code: temp = cs; break;
	case Segment::Data: temp = ds; break;
	case Segment::Stack: temp = ss; break;
	case Segment::Extra: temp = es; break;
	case Segment::Default:
	{
	    switch (index)
	    {
		case 0: temp = cs; break;
		case 1: temp = ds; break;
		case 2: temp = ss; break;
		case 3: temp = es; break;
	    }
	}
	break;
    }

    return temp;
}

auto getSeg(int reg) -> uint16_t
{
    uint16_t data = 0;
    reg &= 3;

    switch (reg)
    {
	case 0: data = es; break;
	case 1: data = cs; break;
	case 2: data = ss; break;
	case 3: data = ds; break;
	default: // This shouldn't happen
	{
	    cout << "Unrecognized segment register number of " << dec << (int)(reg) << endl;
	    exit(1);
	}
 	break;
    }

    return data;
}

auto setSeg(int reg, uint16_t data) -> void
{
    reg &= 3;

    switch (reg)
    {
	case 0: es = data; break;
	case 1: cs = data; break;
	case 2: ss = data; break;
	case 3: ds = data; break;
	default: // This shouldn't happen
	{
	    cout << "Unrecognized segment register number of " << dec << (int)(reg) << endl;
	    exit(1);
	}
 	break;
    }
}

auto decodeModRM(uint8_t byte) -> int
{
    int mod = ((byte >> 6) & 0x3);
    int reg = ((byte >> 3) & 0x7);
    int mem = (byte & 0x7);

    uint16_t segment = 0;
    uint16_t addr = 0;

    int num_cycles = 0;

    if (mod == 3)
    {
	num_cycles = 0;
    }
    else
    {
	if ((mod == 0) && (mem == 6))
	{
	    segment = getSegment(1);
	    addr = getimmWord();
	    num_cycles = 6;
	}
	else
	{
	    switch (mem)
	    {
		case 4:
		{
		    segment = getSegment(1);
		    addr = si;
		    num_cycles = 5;
		}
		break;
		case 5:
		{
		    segment = getSegment(1);
		    addr = di;
		    num_cycles = 5;
		}
		break;
		case 6:
		{
		    segment = getSegment(2);
		    addr = bp;
		    num_cycles = 5;
		}
		break;
		case 7:
		{
		    segment = getSegment(1);
		    addr = bx.getreg();
		    num_cycles = 5;
		}
		break;
		default:
		{
		    cout << "Unrecognized memory selection of " << dec << (int)(mem) << endl;
		    exit(1);
		}
		break;
	    }

	    if (mod == 1)
	    {
		addr += int8_t(getimmByte());
		num_cycles += 4;
	    }
	    else if (mod == 2)
	    {
		addr += int16_t(getimmWord());
		num_cycles += 4;
	    }
	}
    }

    current_mod_rm.mod = mod;
    current_mod_rm.reg = reg;
    current_mod_rm.mem = mem;
    current_mod_rm.segment = segment;
    current_mod_rm.addr = addr;

    if (is_segment_override)
    {
	mem_segment = Segment::Default;
	is_segment_override = false;
    }

    return num_cycles;
}