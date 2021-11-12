#include "NESpp/Debugger.h"
#include "NESpp/Emulator.h"
#include "doctest/doctest.h"

/*
 * Testing all unique instructions, the addressing
 * modes are going to be tested only with STA instruction
 * since they are the same for all instructions
 */

TEST_CASE("CPU executes all instructions correctly")
{
    Emulator testEmulator;
    Debugger testDebugger(testEmulator);

    SUBCASE("CPU can load instructions from array (executes NOP)")
    {
        uint8_t instructions[]{0xEA};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 1);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 5);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x80);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::C>() == 0);
            CHECK(state.PS.Test<CPU::V>() == 1);
        }
    }

    SUBCASE("AND")
    {
        SUBCASE("Result is zero")
        {
            // LDA #$32 ; AND #$E5
            uint8_t instructions[]{0xA9, 0x32, 0x29, 0x00};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x00);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 1);
        }
        SUBCASE("Result is positive")
        {
            // LDA #$32 ; AND #$E5
            uint8_t instructions[]{0xA9, 0x32, 0x29, 0xE5};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x20);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 0);
        }
        SUBCASE("Result is negative")
        {
            // LDA #$32 ; AND #$E5
            uint8_t instructions[]{0xA9, 0x82, 0x29, 0xE5};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x80);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::N>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 0);
        }
    }

    SUBCASE("ASL")
    {
        SUBCASE("Accumulator")
        {
            // LDA #$B6 ; ASL
            uint8_t instructions[]{0xA9, 0xB6, 0x0A};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
            CHECK(state.A == 0x6C);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 3);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::C>() == 1);
        }
        SUBCASE("ZeroPage")
        {
            // LDA #$62 ; STA $EF ; ASL $EF ; LDX $EF
            uint8_t instructions[]{0xA9, 0x62, 0x85, 0xEF, 0x06, 0xEF, 0xA6, 0xEF};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.X == 0xC4);
            CHECK(state.cycleCount == 13);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.PS.Test<CPU::N>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::C>() == 0);
        }
    }

    SUBCASE("BCC")
    {
        SUBCASE("Taken")
        {
            // CLC ; BCC +50
            uint8_t instructions[]{0x18, 0x90, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
            CHECK(state.cycleCount == 5);
            CHECK(state.PC == 0x0700 + 53);
        }
        SUBCASE("Taken with page cross")
        {
            // CLC ; BCC +50
            uint8_t instructions[]{0x18, 0x90, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3, 0x06FA);
            CHECK(state.cycleCount == 6);
            CHECK(state.PC == 0x06FA + 53);
        }
        SUBCASE("Not taken")
        {
            // SEC ; BCC +50
            uint8_t instructions[]{0x38, 0x90, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 3);
        }
    }

    SUBCASE("BCS")
    {
        SUBCASE("Taken")
        {
            // SEC ; BCS +50
            uint8_t instructions[]{0x38, 0xB0, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
            CHECK(state.cycleCount == 5);
            CHECK(state.PC == 0x0700 + 53);
        }
        SUBCASE("Taken with page cross")
        {
            // SEC ; BCS +50
            uint8_t instructions[]{0x38, 0xB0, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3, 0x06FA);
            CHECK(state.cycleCount == 6);
            CHECK(state.PC == 0x06FA + 53);
        }
        SUBCASE("Not taken")
        {
            // CLC ; BCS +50
            uint8_t instructions[]{0x18, 0xB0, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 3);
        }
    }

    SUBCASE("CLC")
    {
        // CLC
        uint8_t instructions[]{0x18};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 1);
        CHECK(state.cycleCount == 2);
        CHECK(state.PC == 0x0700 + 1);
        CHECK(state.PS.Test<CPU::C>() == 0);
    }

    SUBCASE("JMP")
    {
        SUBCASE("Absolute")
        {
            // JMP $03EF
            uint8_t instructions[]{0x4C, 0xEF, 0x07};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
            CHECK(state.cycleCount == 3);
            CHECK(state.PC == 0x07EF);
        }

        SUBCASE("Indirect")
        {
            // LDA #$EF ; STA $0320 ; LDA #$07 ; STA $0321 ; JMP $(0320)
            uint8_t instructions[]{0xA9, 0xEF, 0x8D, 0x20, 0x03, 0xA9, 0x07, 0x8D, 0x21, 0x03, 0x6C, 0x20, 0x03};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 13);
            CHECK(state.cycleCount == 17);
            CHECK(state.PC == 0x07EF);
        }

        SUBCASE("Indirect (with wrap around bug)")
        {
            // LDA #$EF ; STA $03FF ; LDA #$07 ; STA $0321 ; JMP $(0320)
            uint8_t instructions[]{0xA9, 0xEF, 0x8D, 0xFF, 0x03, 0xA9, 0x07, 0x8D, 0x00, 0x03, 0x6C, 0xFF, 0x03};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 13);
            CHECK(state.cycleCount == 17);
            CHECK(state.PC == 0x07EF);
        }
    }

    SUBCASE("LDA")
    {
        SUBCASE("Load zero")
        {
            // LDA #$00
            uint8_t instructions[]{0xA9, 0x00};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 2);
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
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 1);
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
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 6);
            CHECK(state.cycleCount == 8);
            CHECK(state.PC == 0x0700 + 6);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("ZeroPageX (Y)")
        {
            // LDX #$03 ; LDA #$EF ; STA $FE,x ; LDX $01
            uint8_t instructions[]{0xA2, 0x03, 0xA9, 0xEF, 0x95, 0xFE, 0xA6, 0x01};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 11);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("Absolute")
        {
            // LDA #$EF ; STA $0200 ; LDX $0200
            uint8_t instructions[]{0xA9, 0xEF, 0x8D, 0x00, 0x02, 0xAE, 0x00, 0x02};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 10);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("AbsoluteX")
        {
            // LDX #$10 ; LDA #$EF ; STA $0200,x ; LDX $0210
            uint8_t instructions[]{0xA2, 0x10, 0xA9, 0xEF, 0x9D, 0x00, 0x02, 0xAE, 0x10, 0x02};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 10);
            CHECK(state.cycleCount == 13);
            CHECK(state.PC == 0x0700 + 10);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("AbsoluteY")
        {
            // LDY #$10 ; LDA #$EF ; STA $02FE,y ; LDX $0210
            uint8_t instructions[]{0xA0, 0x10, 0xA9, 0xEF, 0x99, 0xFE, 0x02, 0xAE, 0x0E, 0x03};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 10);
            CHECK(state.cycleCount == 13);
            CHECK(state.PC == 0x0700 + 10);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("IndexedIndirect")
        {
            // LDA #$00 ; STA $3F ; LDA #$03 ; STA $40 ; LDA #$EF ; LDX #$20 ; STA ($1F,x) ; LDX $0300
            uint8_t instructions[]{0xA9, 0x00, 0x85, 0x3F, 0xA9, 0x03, 0x85, 0x40, 0xA9,
                                   0xEF, 0xA2, 0x20, 0x81, 0x1F, 0xAE, 0x00, 0x03};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 17);
            CHECK(state.cycleCount == 24);
            CHECK(state.PC == 0x0700 + 17);
            CHECK(state.X == 0xEF);
        }
        SUBCASE("IndirectIndexed")
        {
            // LDA #$00 ; STA $3F ; LDA #$03 ; STA $40 ; LDA #$EF ; LDY #$20 ; STA ($3F),y ; LDX $0320
            uint8_t instructions[]{0xA9, 0x00, 0x85, 0x3F, 0xA9, 0x03, 0x85, 0x40, 0xA9,
                                   0xEF, 0xA0, 0x20, 0x91, 0x3F, 0xAE, 0x20, 0x03};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 17);
            CHECK(state.cycleCount == 24);
            CHECK(state.PC == 0x0700 + 17);
            CHECK(state.X == 0xEF);
        }
    }
}