// Bus header file to define the bus class

#pragma once
#include <cstdint>
#include "cpu6502.h"
#include <array>

class Bus
{
    public:
        // Constructor and Destructor
        Bus();
        ~Bus();

        // Write function to bus that does not return anything, takes a 16-bit address and 8-bit data.
        void write(uint16_t addr, uint8_t data);
        // Read function to bus that returns 8-bit data, takes a 16-bit address and a read only flag.
        uint8_t read(uint16_t addr, bool bReadOnly = false);

        //~~~~~~~~~~~~~~~
        // Components of the bus
        //~~~~~~~~~~~~~~~
        // CPU 6502
        cpu6502 cpu;
        // 64KB RAM
        std::array<uint8_t, 64 * 1024> ram;

};