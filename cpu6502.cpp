// CPU 6502 file
#include "cpu6502.h"
#include "Bus.h"


// Constructor
cpu6502::cpu6502()
{
    // Instruction table
    // https://www.princeton.edu/~mae412/HANDOUTS/Datasheets/6502.pdf (Page 22,23)
    // https://web.archive.org/web/20221112231348if_/http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf (Page 10)
    // { "Instruction", &cpu6502::Opcode, &cpu6502::AddressingMode, Cycles}
    // "&cpu6502::Opcode" a pointer to the opcode in the CPU 6502 class.
    lookup =
    {
		{ "BRK", &cpu6502::BRK, &cpu6502::IMM, 7 },{ "ORA", &cpu6502::ORA, &cpu6502::IZX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 3 },{ "ORA", &cpu6502::ORA, &cpu6502::ZP0, 3 },{ "ASL", &cpu6502::ASL, &cpu6502::ZP0, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },{ "PHP", &cpu6502::PHP, &cpu6502::IMP, 3 },{ "ORA", &cpu6502::ORA, &cpu6502::IMM, 2 },{ "ASL", &cpu6502::ASL, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "ORA", &cpu6502::ORA, &cpu6502::ABS, 4 },{ "ASL", &cpu6502::ASL, &cpu6502::ABS, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },
		{ "BPL", &cpu6502::BPL, &cpu6502::REL, 2 },{ "ORA", &cpu6502::ORA, &cpu6502::IZY, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "ORA", &cpu6502::ORA, &cpu6502::ZPX, 4 },{ "ASL", &cpu6502::ASL, &cpu6502::ZPX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },{ "CLC", &cpu6502::CLC, &cpu6502::IMP, 2 },{ "ORA", &cpu6502::ORA, &cpu6502::ABY, 4 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "ORA", &cpu6502::ORA, &cpu6502::ABX, 4 },{ "ASL", &cpu6502::ASL, &cpu6502::ABX, 7 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },
		{ "JSR", &cpu6502::JSR, &cpu6502::ABS, 6 },{ "AND", &cpu6502::AND, &cpu6502::IZX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "BIT", &cpu6502::BIT, &cpu6502::ZP0, 3 },{ "AND", &cpu6502::AND, &cpu6502::ZP0, 3 },{ "ROL", &cpu6502::ROL, &cpu6502::ZP0, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },{ "PLP", &cpu6502::PLP, &cpu6502::IMP, 4 },{ "AND", &cpu6502::AND, &cpu6502::IMM, 2 },{ "ROL", &cpu6502::ROL, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "BIT", &cpu6502::BIT, &cpu6502::ABS, 4 },{ "AND", &cpu6502::AND, &cpu6502::ABS, 4 },{ "ROL", &cpu6502::ROL, &cpu6502::ABS, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },
		{ "BMI", &cpu6502::BMI, &cpu6502::REL, 2 },{ "AND", &cpu6502::AND, &cpu6502::IZY, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "AND", &cpu6502::AND, &cpu6502::ZPX, 4 },{ "ROL", &cpu6502::ROL, &cpu6502::ZPX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },{ "SEC", &cpu6502::SEC, &cpu6502::IMP, 2 },{ "AND", &cpu6502::AND, &cpu6502::ABY, 4 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "AND", &cpu6502::AND, &cpu6502::ABX, 4 },{ "ROL", &cpu6502::ROL, &cpu6502::ABX, 7 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },
		{ "RTI", &cpu6502::RTI, &cpu6502::IMP, 6 },{ "EOR", &cpu6502::EOR, &cpu6502::IZX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 3 },{ "EOR", &cpu6502::EOR, &cpu6502::ZP0, 3 },{ "LSR", &cpu6502::LSR, &cpu6502::ZP0, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },{ "PHA", &cpu6502::PHA, &cpu6502::IMP, 3 },{ "EOR", &cpu6502::EOR, &cpu6502::IMM, 2 },{ "LSR", &cpu6502::LSR, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "JMP", &cpu6502::JMP, &cpu6502::ABS, 3 },{ "EOR", &cpu6502::EOR, &cpu6502::ABS, 4 },{ "LSR", &cpu6502::LSR, &cpu6502::ABS, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },
		{ "BVC", &cpu6502::BVC, &cpu6502::REL, 2 },{ "EOR", &cpu6502::EOR, &cpu6502::IZY, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "EOR", &cpu6502::EOR, &cpu6502::ZPX, 4 },{ "LSR", &cpu6502::LSR, &cpu6502::ZPX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },{ "CLI", &cpu6502::CLI, &cpu6502::IMP, 2 },{ "EOR", &cpu6502::EOR, &cpu6502::ABY, 4 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "EOR", &cpu6502::EOR, &cpu6502::ABX, 4 },{ "LSR", &cpu6502::LSR, &cpu6502::ABX, 7 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },
		{ "RTS", &cpu6502::RTS, &cpu6502::IMP, 6 },{ "ADC", &cpu6502::ADC, &cpu6502::IZX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 3 },{ "ADC", &cpu6502::ADC, &cpu6502::ZP0, 3 },{ "ROR", &cpu6502::ROR, &cpu6502::ZP0, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },{ "PLA", &cpu6502::PLA, &cpu6502::IMP, 4 },{ "ADC", &cpu6502::ADC, &cpu6502::IMM, 2 },{ "ROR", &cpu6502::ROR, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "JMP", &cpu6502::JMP, &cpu6502::IND, 5 },{ "ADC", &cpu6502::ADC, &cpu6502::ABS, 4 },{ "ROR", &cpu6502::ROR, &cpu6502::ABS, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },
		{ "BVS", &cpu6502::BVS, &cpu6502::REL, 2 },{ "ADC", &cpu6502::ADC, &cpu6502::IZY, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "ADC", &cpu6502::ADC, &cpu6502::ZPX, 4 },{ "ROR", &cpu6502::ROR, &cpu6502::ZPX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },{ "SEI", &cpu6502::SEI, &cpu6502::IMP, 2 },{ "ADC", &cpu6502::ADC, &cpu6502::ABY, 4 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "ADC", &cpu6502::ADC, &cpu6502::ABX, 4 },{ "ROR", &cpu6502::ROR, &cpu6502::ABX, 7 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },
		{ "???", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "STA", &cpu6502::STA, &cpu6502::IZX, 6 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },{ "STY", &cpu6502::STY, &cpu6502::ZP0, 3 },{ "STA", &cpu6502::STA, &cpu6502::ZP0, 3 },{ "STX", &cpu6502::STX, &cpu6502::ZP0, 3 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 3 },{ "DEY", &cpu6502::DEY, &cpu6502::IMP, 2 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "TXA", &cpu6502::TXA, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "STY", &cpu6502::STY, &cpu6502::ABS, 4 },{ "STA", &cpu6502::STA, &cpu6502::ABS, 4 },{ "STX", &cpu6502::STX, &cpu6502::ABS, 4 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 4 },
		{ "BCC", &cpu6502::BCC, &cpu6502::REL, 2 },{ "STA", &cpu6502::STA, &cpu6502::IZY, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },{ "STY", &cpu6502::STY, &cpu6502::ZPX, 4 },{ "STA", &cpu6502::STA, &cpu6502::ZPX, 4 },{ "STX", &cpu6502::STX, &cpu6502::ZPY, 4 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 4 },{ "TYA", &cpu6502::TYA, &cpu6502::IMP, 2 },{ "STA", &cpu6502::STA, &cpu6502::ABY, 5 },{ "TXS", &cpu6502::TXS, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 5 },{ "STA", &cpu6502::STA, &cpu6502::ABX, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },
		{ "LDY", &cpu6502::LDY, &cpu6502::IMM, 2 },{ "LDA", &cpu6502::LDA, &cpu6502::IZX, 6 },{ "LDX", &cpu6502::LDX, &cpu6502::IMM, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },{ "LDY", &cpu6502::LDY, &cpu6502::ZP0, 3 },{ "LDA", &cpu6502::LDA, &cpu6502::ZP0, 3 },{ "LDX", &cpu6502::LDX, &cpu6502::ZP0, 3 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 3 },{ "TAY", &cpu6502::TAY, &cpu6502::IMP, 2 },{ "LDA", &cpu6502::LDA, &cpu6502::IMM, 2 },{ "TAX", &cpu6502::TAX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "LDY", &cpu6502::LDY, &cpu6502::ABS, 4 },{ "LDA", &cpu6502::LDA, &cpu6502::ABS, 4 },{ "LDX", &cpu6502::LDX, &cpu6502::ABS, 4 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 4 },
		{ "BCS", &cpu6502::BCS, &cpu6502::REL, 2 },{ "LDA", &cpu6502::LDA, &cpu6502::IZY, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },{ "LDY", &cpu6502::LDY, &cpu6502::ZPX, 4 },{ "LDA", &cpu6502::LDA, &cpu6502::ZPX, 4 },{ "LDX", &cpu6502::LDX, &cpu6502::ZPY, 4 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 4 },{ "CLV", &cpu6502::CLV, &cpu6502::IMP, 2 },{ "LDA", &cpu6502::LDA, &cpu6502::ABY, 4 },{ "TSX", &cpu6502::TSX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 4 },{ "LDY", &cpu6502::LDY, &cpu6502::ABX, 4 },{ "LDA", &cpu6502::LDA, &cpu6502::ABX, 4 },{ "LDX", &cpu6502::LDX, &cpu6502::ABY, 4 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 4 },
		{ "CPY", &cpu6502::CPY, &cpu6502::IMM, 2 },{ "CMP", &cpu6502::CMP, &cpu6502::IZX, 6 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "CPY", &cpu6502::CPY, &cpu6502::ZP0, 3 },{ "CMP", &cpu6502::CMP, &cpu6502::ZP0, 3 },{ "DEC", &cpu6502::DEC, &cpu6502::ZP0, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },{ "INY", &cpu6502::INY, &cpu6502::IMP, 2 },{ "CMP", &cpu6502::CMP, &cpu6502::IMM, 2 },{ "DEX", &cpu6502::DEX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "CPY", &cpu6502::CPY, &cpu6502::ABS, 4 },{ "CMP", &cpu6502::CMP, &cpu6502::ABS, 4 },{ "DEC", &cpu6502::DEC, &cpu6502::ABS, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },
		{ "BNE", &cpu6502::BNE, &cpu6502::REL, 2 },{ "CMP", &cpu6502::CMP, &cpu6502::IZY, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "CMP", &cpu6502::CMP, &cpu6502::ZPX, 4 },{ "DEC", &cpu6502::DEC, &cpu6502::ZPX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },{ "CLD", &cpu6502::CLD, &cpu6502::IMP, 2 },{ "CMP", &cpu6502::CMP, &cpu6502::ABY, 4 },{ "NOP", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "CMP", &cpu6502::CMP, &cpu6502::ABX, 4 },{ "DEC", &cpu6502::DEC, &cpu6502::ABX, 7 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },
		{ "CPX", &cpu6502::CPX, &cpu6502::IMM, 2 },{ "SBC", &cpu6502::SBC, &cpu6502::IZX, 6 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "CPX", &cpu6502::CPX, &cpu6502::ZP0, 3 },{ "SBC", &cpu6502::SBC, &cpu6502::ZP0, 3 },{ "INC", &cpu6502::INC, &cpu6502::ZP0, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 5 },{ "INX", &cpu6502::INX, &cpu6502::IMP, 2 },{ "SBC", &cpu6502::SBC, &cpu6502::IMM, 2 },{ "NOP", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::SBC, &cpu6502::IMP, 2 },{ "CPX", &cpu6502::CPX, &cpu6502::ABS, 4 },{ "SBC", &cpu6502::SBC, &cpu6502::ABS, 4 },{ "INC", &cpu6502::INC, &cpu6502::ABS, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },
		{ "BEQ", &cpu6502::BEQ, &cpu6502::REL, 2 },{ "SBC", &cpu6502::SBC, &cpu6502::IZY, 5 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 8 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "SBC", &cpu6502::SBC, &cpu6502::ZPX, 4 },{ "INC", &cpu6502::INC, &cpu6502::ZPX, 6 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 6 },{ "SED", &cpu6502::SED, &cpu6502::IMP, 2 },{ "SBC", &cpu6502::SBC, &cpu6502::ABY, 4 },{ "NOP", &cpu6502::NOP, &cpu6502::IMP, 2 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },{ "???", &cpu6502::NOP, &cpu6502::IMP, 4 },{ "SBC", &cpu6502::SBC, &cpu6502::ABX, 4 },{ "INC", &cpu6502::INC, &cpu6502::ABX, 7 },{ "???", &cpu6502::XXX, &cpu6502::IMP, 7 },
	};
}

// Destructor
cpu6502::~cpu6502()
{
    
}

// Read function to CPU 6502 that returns 8-bit data, takes a 16-bit address.
uint8_t cpu6502::read(uint16_t addr)
{
    return bus->read(addr, false);
}

// Write function to CPU 6502 that does not return anything, takes a 16-bit address and 8-bit data.
void cpu6502::write(uint16_t addr, uint8_t data)
{
    bus->write(addr, data);
}

// Clock function to CPU 6502 that does not return anything.
void cpu6502::clock()
{
    // Only excute if cycles is 0
    if (cycles == 0)
    {
        // Set unused flag bit to 1
        SetFlag(U, true);

        // Get the opcode and increment the program counter
        opcode = read(pc);
        pc++;

        // Get required cycles for instruction
        cycles = lookup[opcode].cycles;

        // Get cycles for the addressing mode
        uint8_t additional_cycle1 = (this->*lookup[opcode].addrmode)();

        // Get cycles for the operation and perform the operation
        uint8_t additional_cycle2 = (this->*lookup[opcode].operate)();

        // Add the cycles
        cycles += (additional_cycle1 & additional_cycle2);
    }

    cycles--;
}

// Reset function to CPU 6502 that does not return anything.
void cpu6502::reset()
{
    // Get the address to jump to
    addr_abs = 0xFFFC;
    uint16_t lo = read(addr_abs + 0);
    uint16_t hi = read(addr_abs + 1);

    // Set the program counter to the address
    pc = (hi << 8) | lo;

    // Reset internal registers
    a = 0;
    x = 0;
    y = 0;
    stkp = 0xFD;
    status = 0x00 | U;

    // Clear variables
    addr_rel = 0x0000;
    addr_abs = 0x0000;
    fetched = 0x00;

    // Set cycles required for reset
    cycles = 8;
}

// Interrupt Request
void cpu6502::irq()
{
    // If the interrupt flag is set
    if (GetFlag(I) == 0)
    {
        // Push the program counter to the stack
        write(0x0100 + stkp, (pc >> 8) & 0x00FF);
        stkp--;
        write(0x0100 + stkp, pc & 0x00FF);
        stkp--;

        // Push the status register to the stack
        SetFlag(B, 0);
        SetFlag(U, 1);
        SetFlag(I, 1);
        write(0x0100 + stkp, status);
        stkp--;

        // Get the address to jump to
        addr_abs = 0xFFFE;
        uint16_t lo = read(addr_abs + 0);
        uint16_t hi = read(addr_abs + 1);
        pc = (hi << 8) | lo;

        // Set cycles required for interrupt
        cycles = 7;
    }
}

// Non-Maskable Interrupt Request
void cpu6502::nmi()
{
    // Push the program counter to the stack
    write(0x0100 + stkp, (pc >> 8) & 0x00FF);
    stkp--;
    write(0x0100 + stkp, pc & 0x00FF);
    stkp--;

    // Push the status register to the stack
    SetFlag(B, 0);
    SetFlag(U, 1);
    SetFlag(I, 1);
    write(0x0100 + stkp, status);
    stkp--;

    // Get the address to jump to
    addr_abs = 0xFFFA;
    uint16_t lo = read(addr_abs + 0);
    uint16_t hi = read(addr_abs + 1);
    pc = (hi << 8) | lo;

    // Set cycles required for non-maskable interrupt
    cycles = 8;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Addressing Modes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Implied 
uint8_t cpu6502::IMP()
{
    fetched = a; // Fetch data from the accumulator
    return 0; 
}

// Immediate
uint8_t cpu6502::IMM()
{
    addr_abs = pc++; // Read from the next byte
    return 0;
}

// Zero page
uint8_t cpu6502::ZP0()
{
    addr_abs = read(pc); 
    pc++;
    addr_abs &= 0x00FF; 
    return 0;
}

// Zero page with X offset
uint8_t cpu6502::ZPX()
{
    addr_abs = (read(pc) + x); 
    pc++;
    addr_abs &= 0x00FF; 
    return 0;
}

// Zero page with Y offset
uint8_t cpu6502::ZPY()
{
    addr_abs = (read(pc) + y); 
    pc++;
    addr_abs &= 0x00FF; 
    return 0;
}

// Absolute
uint8_t cpu6502::ABS()
{
    uint16_t lo = read(pc); 
    pc++;
    uint16_t hi = read(pc); 
    pc++;

    addr_abs = (hi << 8) | lo; 
    return 0;
}

// Absolute with X offset
uint8_t cpu6502::ABX()
{
    uint16_t lo = read(pc); 
    pc++;
    uint16_t hi = read(pc); 
    pc++;

    addr_abs = (hi << 8) | lo; 
    addr_abs += x; 

    if ((addr_abs & 0xFF00) != (hi << 8))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Absolute with Y offset
uint8_t cpu6502::ABY()
{
    uint16_t lo = read(pc); 
    pc++;
    uint16_t hi = read(pc); 
    pc++;

    addr_abs = (hi << 8) | lo; 
    addr_abs += y; 

    if ((addr_abs & 0xFF00) != (hi << 8))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Indirect
uint8_t cpu6502::IND()
{
    uint16_t ptr_lo = read(pc); 
    pc++;
    uint16_t ptr_hi = read(pc); 
    pc++;

    uint16_t ptr = (ptr_hi << 8) | ptr_lo; 

    if (ptr_lo == 0x00FF) 
    {
        addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0); 
    }
    else 
    {
        addr_abs = (read(ptr + 1) << 8) | read(ptr + 0); 
    }

    return 0;
}

// Indirect X
uint8_t cpu6502::IZX()
{
    uint16_t t = read(pc); 
    pc++;

    uint16_t lo = read((uint16_t)(t + (uint16_t)x) & 0x00FF); 
    uint16_t hi = read((uint16_t)(t + (uint16_t)x + 1) & 0x00FF); 

    addr_abs = (hi << 8) | lo; 

    return 0;
}

// Indirect Y
uint8_t cpu6502::IZY()
{
    uint16_t t = read(pc); 
    pc++;

    uint16_t lo = read(t & 0x00FF); 
    uint16_t hi = read((t + 1) & 0x00FF); 

    addr_abs = (hi << 8) | lo; 
    addr_abs += y; 

    if ((addr_abs & 0xFF00) != (hi << 8))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Relative
uint8_t cpu6502::REL()
{
    addr_rel = read(pc); 
    pc++;
    if (addr_rel & 0x80)
    {
        addr_rel |= 0xFF00;
    }
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Instructions
// https://www.princeton.edu/~mae412/HANDOUTS/Datasheets/6502.pdf (Page 23)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Fetch
uint8_t cpu6502::fetch()
{
    if (!(lookup[opcode].addrmode == &cpu6502::IMP))
    {
        fetched = read(addr_abs);
    }
    return fetched;
}

//Add Memory to Accumulator with Carry
uint8_t cpu6502::ADC()
{
    fetch(); // Fetch data
    uint16_t temp = (uint16_t)a + (uint16_t)fetched + (uint16_t)GetFlag(C); // Perform addition
    SetFlag(N, temp & 0x80); // Set negative flag
    SetFlag(V, (~(uint16_t)a ^ (uint16_t)fetched) & ((uint16_t)a ^ (uint16_t)temp) & 0x0080); // Set overflow flag
    SetFlag(Z, (temp & 0x00FF) == 0); // Set zero flag
    SetFlag(C, temp > 255); // Set carry flag
}

// "AND" Memory with Accumulator
uint8_t cpu6502::AND()
{
    fetch(); // Fetch data
    a = a & fetched; // Perform AND operation
    SetFlag(N, a & 0x80); // Set negative flag 
    SetFlag(Z, a == 0x00); // Set zero flag
    return 1; // Return 1 cycle
}

// Shift One Bit Left
uint8_t cpu6502::ASL()
{
    fetch(); // Fetch data
    uint16_t temp = (uint16_t)fetched << 1; // Shift left by 1
    SetFlag(C, (temp & 0xFF00) > 0); // Set carry flag
    SetFlag(Z, (temp & 0x00FF) == 0x00); // Set zero flag
    SetFlag(N, temp & 0x80); // Set negative flag

    if (lookup[opcode].addrmode == &cpu6502::IMP)
    {
        a = temp & 0x00FF; // Set accumulator to temp
    }
    else
    {
        write(addr_abs, temp & 0x00FF); // Write temp to memory
    }
    return 0; // Return 0 cycles
}

// Branch on Carry Clear
uint8_t cpu6502::BCC()
{
    if (GetFlag(C) == 0) // Branch if carry flag is clear
    {
        cycles++;
        addr_abs = pc + addr_rel;

        if ((addr_abs & 0xFF00) != (pc & 0xFF00)) // If you have to cross a page boundary, add an extra cycle
        {
            cycles++;
        }

        pc = addr_abs;
    }
    return 0;
}

// Branch on Carry Set
uint8_t cpu6502::BCS()
{
    if (GetFlag(C) == 1) // Branch if carry flag is set
    {
        cycles++;
        addr_abs = pc + addr_rel; // Set the address to the program counter plus offset

        if ((addr_abs & 0xFF00) != (pc & 0xFF00)) // If you have to cross a page boundary, add an extra cycle
        {
            cycles++;
        }

        pc = addr_abs; // Set the program counter to the address
    }
    return 0;
}

// Branch on Equal
uint8_t cpu6502::BEQ()
{
    if (GetFlag(Z) == 1) // Branch if zero flag is set
    {
        cycles++;
        addr_abs = pc + addr_rel; // Set the address to the program counter plus offset

        if ((addr_abs & 0xFF00) != (pc & 0xFF00)) // If you have to cross a page boundary, add an extra cycle
        {
            cycles++;
        }

        pc = addr_abs; // Set the program counter to the address
    }
    return 0;
}

// Test Memory Bits with Accumlator
uint8_t cpu6502::BIT()
{
    fetch(); // Fetch data

    uint16_t temp = a & fetched; // Perform AND operation

    SetFlag(N, fetched & (1 << 7)); // Set negative flag
    SetFlag(V, fetched & (1 << 6)); // Set overflow flag
    SetFlag(Z, (temp & 0x00FF) == 0x00); // Set zero flag

    return 0; // Return 0 cycles
}

// Branch on Result Minus
uint8_t cpu6502::BMI()
{
    if (GetFlag(N) == 1) // Branch if negative flag is set
    {
        cycles++;
        addr_abs = pc + addr_rel; // Set the address to the program counter plus offset

        if ((addr_abs & 0xFF00) != (pc & 0xFF00)) // If you have to cross a page boundary, add an extra cycle
        {
            cycles++;
        }

        pc = addr_abs; // Set the program counter to the address
    }
    return 0;
}

// Branch on Result Not Zero
uint8_t cpu6502::BNE()
{
    if (GetFlag(Z) == 0) // Branch if zero flag is clear
    {
        cycles++;
        addr_abs = pc + addr_rel; // Set the address to the program counter plus offset

        if ((addr_abs & 0xFF00) != (pc & 0xFF00)) // If you have to cross a page boundary, add an extra cycle
        {
            cycles++;
        }

        pc = addr_abs; // Set the program counter to the address
    }
    return 0;
}

// Branch on Result Plus
uint8_t cpu6502::BPL()
{
    if (GetFlag(N) == 0) // Branch if negative flag is clear
    {
        cycles++;
        addr_abs = pc + addr_rel; // Set the address to the program counter plus offset

        if ((addr_abs & 0xFF00) != (pc & 0xFF00)) // If you have to cross a page boundary, add an extra cycle
        {
            cycles++;
        }

        pc = addr_abs; // Set the program counter to the address
    }
    return 0;
}

/* BRA - Branch Always (Not Implemented)
uint8_t cpu6502::BRA()
{
    addr_abs = pc + addr_rel; // Set the address to the program counter plus offset
    pc = addr_abs; // Set the program counter to the address
    return 0; // Return 0 cycles
}
*/

// Force Break
uint8_t cpu6502::BRK()
{
    pc++; // Increment the program counter
    SetFlag(I, true); // Set interrupt flag
    write(0x0100 + stkp, (pc >> 8) & 0x00FF); // Write the program counter to the stack
    stkp--; // Decrement the stack pointer
    write(0x0100 + stkp, pc & 0x00FF); // Write the program counter to the stack
    stkp--; // Decrement the stack pointer

    SetFlag(B, true); // Set break flag
    write(0x0100 + stkp, status); // Write the status register to the stack
    stkp--; // Decrement the stack pointer
    SetFlag(B, false); // Clear break flag
    SetFlag(U, true); // Set unused flag
    SetFlag(I, true); // Set interrupt flag

    pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8); // Set the program counter to the interrupt vector
    return 0; // Return 0 cycles
}

// Branch on Overflow Clear
uint8_t cpu6502::BVC()
{
    if (GetFlag(V) == 0) // Branch if overflow flag is clear
    {
        cycles++;
        addr_abs = pc + addr_rel; // Set the address to the program counter plus offset

        if ((addr_abs & 0xFF00) != (pc & 0xFF00)) // If you have to cross a page boundary, add an extra cycle
        {
            cycles++;
        }

        pc = addr_abs; // Set the program counter to the address
    }
    return 0;
}

// Branch on Overflow Set
uint8_t cpu6502::BVS()
{
    if (GetFlag(V) == 1) // Branch if overflow flag is set
    {
        cycles++;
        addr_abs = pc + addr_rel; // Set the address to the program counter plus offset

        if ((addr_abs & 0xFF00) != (pc & 0xFF00)) // If you have to cross a page boundary, add an extra cycle
        {
            cycles++;
        }

        pc = addr_abs; // Set the program counter to the address
    }
    return 0;
}

// Clear Carry Flag
uint8_t cpu6502::CLC()
{
    SetFlag(C, false); // Clear carry flag
    return 0; // Return 0 cycles
}

// Clear Decimal Mode
uint8_t cpu6502::CLD()
{
    SetFlag(D, false); // Clear decimal flag
    return 0; // Return 0 cycles
}

// Clear Interrupt Disable Bit
uint8_t cpu6502::CLI()
{
    SetFlag(I, false); // Clear interrupt flag
    return 0; // Return 0 cycles
}

// Clear Overflow Flag
uint8_t cpu6502::CLV()
{
    SetFlag(V, false); // Clear overflow flag
    return 0; // Return 0 cycles
}

// Compare Memory and Accumulator
uint8_t cpu6502::CMP()
{
    fetch(); // Fetch data
    uint16_t temp = (uint16_t)a - (uint16_t)fetched; // Perform subtraction
    SetFlag(N, temp & 0x80); // Set negative flag
    SetFlag(Z, (temp & 0x00FF) == 0); // Set zero flag
    SetFlag(C, a >= fetched); // Set carry flag
    return 1; // Return 1 cycle
}

// Compare Memory and Index X
uint8_t cpu6502::CPX()
{
    fetch(); // Fetch data
    uint16_t temp = (uint16_t)x - (uint16_t)fetched; // Perform subtraction
    SetFlag(N, temp & 0x80); // Set negative flag
    SetFlag(Z, (temp & 0x00FF) == 0); // Set zero flag
    SetFlag(C, x >= fetched); // Set carry flag
    return 0; // Return 0 cycles
}

// Compare Memory and Index Y
uint8_t cpu6502::CPY()
{
    fetch(); // Fetch data
    uint16_t temp = (uint16_t)y - (uint16_t)fetched; // Perform subtraction
    SetFlag(N, temp & 0x80); // Set negative flag
    SetFlag(Z, (temp & 0x00FF) == 0); // Set zero flag
    SetFlag(C, y >= fetched); // Set carry flag
    return 0; // Return 0 cycles
}

// Decrement by One
uint8_t cpu6502::DEC()
{
    fetch(); // Fetch data
    uint16_t temp = fetched - 1; // Decrement by 1
    write(addr_abs, temp & 0x00FF); // Write temp to memory
    SetFlag(N, temp & 0x80); // Set negative flag
    SetFlag(Z, (temp & 0x00FF) == 0); // Set zero flag
    return 0; // Return 0 cycles
}

// Decrement Index X by One
uint8_t cpu6502::DEX()
{
    x--; // Decrement by 1
    SetFlag(N, x & 0x80); // Set negative flag
    SetFlag(Z, x == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

// Decrement Index Y by One
uint8_t cpu6502::DEY()
{
    y--; // Decrement by 1
    SetFlag(N, y & 0x80); // Set negative flag
    SetFlag(Z, y == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

// "Exclusive-OR" Memory with Accumulator
uint8_t cpu6502::EOR()
{
    fetch(); // Fetch data
    a = a ^ fetched; // Perform XOR operation
    SetFlag(N, a & 0x80); // Set negative flag
    SetFlag(Z, a == 0x00); // Set zero flag
    return 1; // Return 1 cycle
}

// Increment Memory by One
uint8_t cpu6502::INC()
{
    fetch(); // Fetch data
    uint16_t temp = fetched + 1; // Increment by 1
    write(addr_abs, temp & 0x00FF); // Write temp to memory
    SetFlag(N, temp & 0x80); // Set negative flag
    SetFlag(Z, (temp & 0x00FF) == 0); // Set zero flag
    return 0; // Return 0 cycles
}

// Increment Index X by One
uint8_t cpu6502::INX()
{
    x++; // Increment by 1
    SetFlag(N, x & 0x80); // Set negative flag
    SetFlag(Z, x == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

// Increment Index Y by One
uint8_t cpu6502::INY()
{
    y++; // Increment by 1
    SetFlag(N, y & 0x80); // Set negative flag
    SetFlag(Z, y == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

// Jump to New Location
uint8_t cpu6502::JMP()
{
    pc = addr_abs; // Set the program counter to the address
    return 0; // Return 0 cycles
}

// Jump to New Location Saving Return Address
uint8_t cpu6502::JSR()
{
    pc--; // Decrement the program counter
    write(0x0100 + stkp, (pc >> 8) & 0x00FF); // Write the program counter to the stack
    stkp--; // Decrement the stack pointer
    write(0x0100 + stkp, pc & 0x00FF); // Write the program counter to the stack
    stkp--; // Decrement the stack pointer

    pc = addr_abs; // Set the program counter to the address
    return 0; // Return 0 cycles
}

// Load Accumulator with Memory
uint8_t cpu6502::LDA()
{
    fetch(); // Fetch data
    a = fetched; // Set accumulator to fetched
    SetFlag(N, a & 0x80); // Set negative flag
    SetFlag(Z, a == 0x00); // Set zero flag
    return 1; // Return 1 cycle
}

// Load Index X with Memory
uint8_t cpu6502::LDX()
{
    fetch(); // Fetch data
    x = fetched; // Set index X to fetched
    SetFlag(N, x & 0x80); // Set negative flag
    SetFlag(Z, x == 0x00); // Set zero flag
    return 1; // Return 1 cycle
}

// Load Index Y with Memory
uint8_t cpu6502::LDY()
{
    fetch(); // Fetch data
    y = fetched; // Set index Y to fetched
    SetFlag(N, y & 0x80); // Set negative flag
    SetFlag(Z, y == 0x00); // Set zero flag
    return 1; // Return 1 cycle
}

// Shift One Bit Right
uint8_t cpu6502::LSR()
{
    fetch(); // Fetch data
    SetFlag(C, fetched & 0x0001); // Set carry flag
    uint16_t temp = fetched >> 1; // Shift right by 1
    SetFlag(N, temp & 0x80); // Set negative flag
    SetFlag(Z, (temp & 0x00FF) == 0x00); // Set zero flag

    if (lookup[opcode].addrmode == &cpu6502::IMP)
    {
        a = temp & 0x00FF; // Set accumulator to temp
    }
    else
    {
        write(addr_abs, temp & 0x00FF); // Write temp to memory
    }
    return 0; // Return 0 cycles
}

// No Operation
uint8_t cpu6502::NOP()
{
    switch (opcode) 
    {
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
            return 1;
            break;
    }
    return 0; // Return 0 cycles
}

// "OR" Memory with Accumulator
uint8_t cpu6502::ORA()
{
    fetch(); // Fetch data
    a = a | fetched; // Perform OR operation
    SetFlag(N, a & 0x80); // Set negative flag
    SetFlag(Z, a == 0x00); // Set zero flag
    return 1; // Return 1 cycle
}

// Push Accumulator on Stack
uint8_t cpu6502::PHA()
{
    write(0x0100 + stkp, a); // Write accumulator to the stack
    stkp--; // Decrement the stack pointer
    return 0; // Return 0 cycles
}

// Push Processor Status on Stack
uint8_t cpu6502::PHP()
{
    write(0x0100 + stkp, status | B | U); // Write the status register to the stack
    SetFlag(B, false); // Clear break flag
    SetFlag(U, true); // Set unused flag
    stkp--; // Decrement the stack pointer
    return 0; // Return 0 cycles
}

/* PHX - Push Index X on Stack (Not Implemented)
uint8_t cpu6502::PHX()
{
    write(0x0100 + stkp, x); // Write index X to the stack
    stkp--; // Decrement the stack pointer
    return 0; // Return 0 cycles
}
*/


/* PHY - Push Index Y on Stack (Not Implemented)
uint8_t cpu6502::PHY()
{
    write(0x0100 + stkp, y); // Write index Y to the stack
    stkp--; // Decrement the stack pointer
    return 0; // Return 0 cycles
}
*/

// Pull Accumulator from Stack
uint8_t cpu6502::PLA()
{
    stkp++; // Increment the stack pointer
    a = read(0x0100 + stkp); // Read the accumulator from the stack
    SetFlag(N, a & 0x80); // Set negative flag
    SetFlag(Z, a == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

// Pull Processor Status from Stack
uint8_t cpu6502::PLP()
{
    stkp++; // Increment the stack pointer
    status = read(0x0100 + stkp); // Read the status register from the stack
    SetFlag(U, true); // Set unused flag
    return 0; // Return 0 cycles
}

/* PLX - Pull Index X from Stack (Not Implemented)
uint8_t cpu6502::PLX()
{
    stkp++; // Increment the stack pointer
    x = read(0x0100 + stkp); // Read index X from the stack
    SetFlag(N, x & 0x80); // Set negative flag
    SetFlag(Z, x == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}
*/

/* PLY - Pull Index Y from Stack (Not Implemented)
uint8_t cpu6502::PLY()
{
    stkp++; // Increment the stack pointer
    y = read(0x0100 + stkp); // Read index Y from the stack
    SetFlag(N, y & 0x80); // Set negative flag
    SetFlag(Z, y == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}
*/

// Rotate One Bit Left
uint8_t cpu6502::ROL()
{
    fetch(); // Fetch data
    uint16_t temp = (uint16_t)(fetched << 1) | GetFlag(C); // Shift left by 1
    SetFlag(C, temp & 0xFF00); // Set carry flag
    SetFlag(Z, (temp & 0x00FF) == 0x00); // Set zero flag
    SetFlag(N, temp & 0x80); // Set negative flag

    if (lookup[opcode].addrmode == &cpu6502::IMP)
    {
        a = temp & 0x00FF; // Set accumulator to temp
    }
    else
    {
        write(addr_abs, temp & 0x00FF); // Write temp to memory
    }
    return 0; // Return 0 cycles
}

// Rotate One Bit Right
uint8_t cpu6502::ROR()
{
    fetch(); // Fetch data
    uint16_t temp = (uint16_t)(GetFlag(C) << 7) | (fetched >> 1); // Shift right by 1
    SetFlag(C, fetched & 0x01); // Set carry flag
    SetFlag(Z, (temp & 0x00FF) == 0x00); // Set zero flag
    SetFlag(N, temp & 0x80); // Set negative flag

    if (lookup[opcode].addrmode == &cpu6502::IMP)
    {
        a = temp & 0x00FF; // Set accumulator to temp
    }
    else
    {
        write(addr_abs, temp & 0x00FF); // Write temp to memory
    }
    return 0; // Return 0 cycles
}

// Return from Interrupt
uint8_t cpu6502::RTI()
{
    stkp++; // Increment the stack pointer
    status = read(0x0100 + stkp); // Read the status register from the stack
    status &= ~B; // Clear break flag
    status &= ~U; // Clear unused flag

    stkp++; // Increment the stack pointer
    pc = (uint16_t)read(0x0100 + stkp); // Read the program counter from the stack
    stkp++; // Increment the stack pointer
    pc |= (uint16_t)read(0x0100 + stkp) << 8; // Read the program counter from the stack
    return 0; // Return 0 cycles
}

// Return from Subroutine
uint8_t cpu6502::RTS()
{
    stkp++; // Increment the stack pointer
    pc = (uint16_t)read(0x0100 + stkp); // Read the program counter from the stack
    stkp++; // Increment the stack pointer
    pc |= (uint16_t)read(0x0100 + stkp) << 8; // Read the program counter from the stack
    pc++; // Increment the program counter
    return 0; // Return 0 cycles
}

// Subtract Memory from Accumulator with Borrow
uint8_t cpu6502::SBC()
{
    fetch(); // Fetch data

    uint16_t value = (uint16_t)fetched ^ 0x00FF; // Perform bitwise XOR operation
    uint16_t temp = (uint16_t)a + value + (uint16_t)GetFlag(C); // Perform addition

    SetFlag(N, temp & 0x80); // Set negative flag
    SetFlag(V, (~(uint16_t)a ^ value) & ((uint16_t)a ^ temp) & 0x0080); // Set overflow flag
    SetFlag(Z, (temp & 0x00FF) == 0); // Set zero flag
    SetFlag(C, temp & 0x8000); // Set carry flag

    a = temp & 0x00FF; // Set accumulator to temp
    return 1; // Return 1 cycle
}

// Set Carry Flag
uint8_t cpu6502::SEC()
{
    SetFlag(C, true); // Set carry flag
    return 0; // Return 0 cycles
}

// Set Decimal Flag
uint8_t cpu6502::SED()
{
    SetFlag(D, true); // Set decimal flag
    return 0; // Return 0 cycles
}

// Set Interrupt Disable Bit
uint8_t cpu6502::SEI()
{
    SetFlag(I, true); // Set interrupt flag
    return 0; // Return 0 cycles
}

// Store Accumulator in Memory
uint8_t cpu6502::STA()
{
    write(addr_abs, a); // Write accumulator to memory
    return 0; // Return 0 cycles
}

// Store Index X in Memory
uint8_t cpu6502::STX()
{
    write(addr_abs, x); // Write index X to memory
    return 0; // Return 0 cycles
}

// Store Index Y in Memory
uint8_t cpu6502::STY()
{
    write(addr_abs, y); // Write index Y to memory
    return 0; // Return 0 cycles
}

/* STZ - Store Zero in Memory (Not Implemented)
uint8_t cpu6502::STZ()
{
    write(addr_abs, 0x00); // Write 0 to memory
    return 0; // Return 0 cycles
}
*/

// Transfer Accumulator to Index X
uint8_t cpu6502::TAX()
{
    x = a; // Set index X to accumulator
    SetFlag(N, x & 0x80); // Set negative flag
    SetFlag(Z, x == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

// Transfer Accumulator to Index Y
uint8_t cpu6502::TAY()
{
    y = a; // Set index Y to accumulator
    SetFlag(N, y & 0x80); // Set negative flag
    SetFlag(Z, y == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

/* TRB - Test and Reset Memory Bits with Accumulator (Not Implemented)
uint8_t cpu6502::TRB()
{
    fetch(); // Fetch data
    uint16_t temp = a & fetched; // Perform AND operation
    SetFlag(Z, (temp & 0x00FF) == 0x00); // Set zero flag
    write(addr_abs, fetched & ~a); // Write the bitwise NOT of the accumulator to memory
    return 0; // Return 0 cycles
}
*/

/* TSB - Test and Set Memory Bits with Accumulator (Not Implemented)
uint8_t cpu6502::TSB()
{
    fetch(); // Fetch data
    uint16_t temp = a & fetched; // Perform AND operation
    SetFlag(Z, (temp & 0x00FF) == 0x00); // Set zero flag
    write(addr_abs, fetched | a); // Write the bitwise OR of the accumulator to memory
    return 0; // Return 0 cycles
}
*/

// Transfer Stack Pointer to Index X
uint8_t cpu6502::TSX()
{
    x = stkp; // Set index X to stack pointer
    SetFlag(N, x & 0x80); // Set negative flag
    SetFlag(Z, x == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

// Transfer Index X to Accumulator
uint8_t cpu6502::TXA()
{
    a = x; // Set accumulator to index X
    SetFlag(N, a & 0x80); // Set negative flag
    SetFlag(Z, a == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

// Transfer Index X to Stack Pointer
uint8_t cpu6502::TXS()
{
    stkp = x; // Set stack pointer to index X
    return 0; // Return 0 cycles
}

// Transfer Index Y to Accumulator
uint8_t cpu6502::TYA()
{
    a = y; // Set accumulator to index Y
    SetFlag(N, a & 0x80); // Set negative flag
    SetFlag(Z, a == 0x00); // Set zero flag
    return 0; // Return 0 cycles
}

// Illegal Opcodes
uint8_t cpu6502::XXX()
{
    return 0; // Return 0 cycles
}
