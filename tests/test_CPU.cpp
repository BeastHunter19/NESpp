#include "NESpp/NES.h"
#include "doctest/doctest.h"

/*
 * Testing all unique instructions, the addressing
 * modes are going to be tested only with STA instruction
 * since they are the same for all instructions
 */

TEST_CASE("CPU executes all instructions correctly")
{
    NES testEmulator;

    SUBCASE("CPU can load instructions from array (executes NOP)")
    {
        uint8_t instructions[]{0xEA};
        CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 1);
        CHECK(state.cycleCount == 2);
        CHECK(state.PC == 0x0700 + 1);
        CHECK(state.SP == 0xFD);
        CHECK(state.PS.value == 0x34);
    }

    SUBCASE("ADC")
    {
        SUBCASE("Add to empty accumulator")
        {
            // ADC #$30
            uint8_t instructions[]{0x69, 0x30};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.A == 0x30);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.PS.Test<CPU::C>() == 0);
            CHECK(state.PS.Test<CPU::V>() == 0);
        }
        SUBCASE("Add with carry unset")
        {
            // LDA #$11 ; ADC #$1F
            uint8_t instructions[]{0xA9, 0x11, 0x69, 0x1F};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x30);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::C>() == 0);
            CHECK(state.PS.Test<CPU::V>() == 0);
        }
        SUBCASE("Add with carry set")
        {
            // SEC ; LDA #$11 ; ADC #$1F
            uint8_t instructions[]{0x38, 0xA9, 0x11, 0x69, 0x1F};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 5);
            CHECK(state.A == 0x31);
            CHECK(state.cycleCount == 6);
            CHECK(state.PC == 0x0700 + 5);
            CHECK(state.PS.Test<CPU::C>() == 0);
            CHECK(state.PS.Test<CPU::V>() == 0);
        }
        SUBCASE("Add triggers carry")
        {
            // LDA #$FE ; ADC #$05
            uint8_t instructions[]{0xA9, 0xFE, 0x69, 0x05};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x03);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::C>() == 1);
            CHECK(state.PS.Test<CPU::V>() == 0);
        }
        SUBCASE("Add triggers overflow")
        {
            // LDA #$7F ; ADC #$01
            uint8_t instructions[]{0xA9, 0x7F, 0x69, 0x01};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x80);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::C>() == 0);
            CHECK(state.PS.Test<CPU::V>() == 1);
        }
    }

    SUBCASE("CLC")
    {
        // CLC
        uint8_t instructions[]{0x18};
        CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 1);
        CHECK(state.cycleCount == 2);
        CHECK(state.PC == 0x0700 + 1);
        CHECK(state.PS.Test<CPU::C>() == 0);
    }

    SUBCASE("LDA")
    {
        SUBCASE("Load zero")
        {
            // LDA #$00
            uint8_t instructions[]{0xA9, 0x00};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.A == 0x00);
            CHECK(state.PS.Test<CPU::Z>() == 1);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("Load positive")
        {
            // LDA #$7E
            uint8_t instructions[]{0xA9, 0x7E};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.A == 0x7E);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("Load negative")
        {
            // LDA #$80
            uint8_t instructions[]{0xA9, 0x80};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.A == 0x80);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 1);
        }
    }

    SUBCASE("LDX")
    {
        SUBCASE("Load zero")
        {
            // LDX #$00
            uint8_t instructions[]{0xA2, 0x00};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.X == 0x00);
            CHECK(state.PS.Test<CPU::Z>() == 1);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("Load positive")
        {
            // LDX #$7E
            uint8_t instructions[]{0xA2, 0x7E};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.X == 0x7E);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("Load negative")
        {
            // LDX #$80
            uint8_t instructions[]{0xA2, 0x80};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.X == 0x80);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 1);
        }
    }

    SUBCASE("LDY")
    {
        SUBCASE("Load zero")
        {
            // LDY #$00
            uint8_t instructions[]{0xA0, 0x00};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.Y == 0x00);
            CHECK(state.PS.Test<CPU::Z>() == 1);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("Load positive")
        {
            // LDY #$7E
            uint8_t instructions[]{0xA0, 0x7E};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.Y == 0x7E);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("Load negative")
        {
            // LDY #$80
            uint8_t instructions[]{0xA0, 0x80};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 2);
            CHECK(state.cycleCount == 2);
            CHECK(state.PC == 0x0700 + 2);
            CHECK(state.Y == 0x80);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 1);
        }
    }

    SUBCASE("SEC")
    {
        // SEC
        uint8_t instructions[]{0x38};
        CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 1);
        CHECK(state.cycleCount == 2);
        CHECK(state.PC == 0x0700 + 1);
        CHECK(state.PS.Test<CPU::C>() == 1);
    }

    SUBCASE("STA")
    {
        SUBCASE("ZeroPage")
        {
            // LDA #$EF ; STA $1F ; LDX $1F
            uint8_t instructions[]{0xA9, 0xEF, 0x85, 0x1F, 0xA6, 0x1F};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 6);
            CHECK(state.cycleCount == 8);
            CHECK(state.PC == 0x0700 + 6);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("ZeroPageX (Y)")
        {
            // LDX #$03 ; LDA #$EF ; STA $FE,x ; LDX $01
            uint8_t instructions[]{0xA2, 0x03, 0xA9, 0xEF, 0x95, 0xFE, 0xA6, 0x01};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 11);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("Absolute")
        {
            // LDA #$EF ; STA $0200 ; LDX $0200
            uint8_t instructions[]{0xA9, 0xEF, 0x8D, 0x00, 0x02, 0xAE, 0x00, 0x02};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 10);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("AbsoluteX")
        {
            // LDX #$10 ; LDA #$EF ; STA $0200,x ; LDX $0210
            uint8_t instructions[]{0xA2, 0x10, 0xA9, 0xEF, 0x9D, 0x00, 0x02, 0xAE, 0x10, 0x02};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 10);
            CHECK(state.cycleCount == 13);
            CHECK(state.PC == 0x0700 + 10);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("AbsoluteY")
        {
            // LDY #$10 ; LDA #$EF ; STA $02FE,y ; LDX $0210
            uint8_t instructions[]{0xA0, 0x10, 0xA9, 0xEF, 0x99, 0xFE, 0x02, 0xAE, 0x0E, 0x03};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 10);
            CHECK(state.cycleCount == 13);
            CHECK(state.PC == 0x0700 + 10);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("IndexedIndirect")
        {
            // LDA #$00 ; STA $3F ; LDA #$03 ; STA $40 ; LDA #$EF ; LDX #$20 ; STA ($1F,x) ; LDX $0300
            uint8_t instructions[]{0xA9, 0x00, 0x85, 0x3F, 0xA9, 0x03, 0x85, 0x40, 0xA9,
                                   0xEF, 0xA2, 0x20, 0x81, 0x1F, 0xAE, 0x00, 0x03};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 17);
            CHECK(state.cycleCount == 24);
            CHECK(state.PC == 0x0700 + 17);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("IndirectIndexed")
        {
            // LDA #$00 ; STA $3F ; LDA #$03 ; STA $40 ; LDA #$EF ; LDY #$20 ; STA ($3F),y ; LDX $0320
            uint8_t instructions[]{0xA9, 0x00, 0x85, 0x3F, 0xA9, 0x03, 0x85, 0x40, 0xA9,
                                   0xEF, 0xA0, 0x20, 0x91, 0x3F, 0xAE, 0x20, 0x03};
            CPU::CpuState state = testEmulator.ExecuteInstrFromArray(instructions, 17);
            CHECK(state.cycleCount == 24);
            CHECK(state.PC == 0x0700 + 17);
            CHECK(state.X == 0xEF);
        }
    }
}