#include "NES.h"
#include "doctest/doctest.h"

TEST_CASE("CPU executes all instructions correctly")
{
    NES testEmulator;

    SUBCASE("CPU can load instructions from array (executes NOP)")
    {
        uint8_t instructions[]{0xEA};
        CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 1);
        CHECK(state.cycleCount == 2);
        CHECK(state.PC == 1);
    }
}