// File that acts as a bus for the program
#include "Bus.h"

// Constructor
Bus::Bus()
{
    /* 
    Clear RAM
    In clearer terms, for each element in the RAM, set the actual value to 0x00. 
    "auto &i" is a reference to each element in the array.
    "auto" deduces the type of i from the type of elements in "ram".
    "&" means that i is a reference, so we can modify the actual element in the array.   
    */
    for(auto &i : ram) i = 0x00;

    // Connect the CPU to the bus
    cpu.ConnectBus(this);

}

// Destructor
Bus::~Bus()
{
    
}

// Write function to bus that does not return anything, takes a 16-bit address and 8-bit data.
void Bus::write(uint16_t addr, uint8_t data)
{   
    // If the address is within the range of the RAM, write the data to the address.
    if (addr >= 0x0000 && addr <= 0xFFFF)
    {
        ram[addr] = data;
    }
}

// Read function to bus that returns 8-bit data, takes a 16-bit address and a read only flag.
uint8_t Bus::read(uint16_t addr, bool bReadOnly)
{
    // If the address is within the range of the RAM, return the data at the address.
    if (addr >= 0x0000 && addr <= 0xFFFF)
    {
        return ram[addr];
    }

    // If the address is not within the range of the RAM, return 0x00.
    return 0x00;
}
