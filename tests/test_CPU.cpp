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
            // LDA #$32 ; AND #$00
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
            // LDA #$82 ; AND #$E5
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

    SUBCASE("BEQ")
    {
        SUBCASE("Taken")
        {
            // AND #$00 ; BEQ +50
            uint8_t instructions[]{0x29, 0x00, 0xF0, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 5);
            CHECK(state.PC == 0x0700 + 54);
        }
        SUBCASE("Taken with page cross")
        {
            // AND #$00 ; BEQ +50
            uint8_t instructions[]{0x29, 0x00, 0xF0, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4, 0x06FA);
            CHECK(state.cycleCount == 6);
            CHECK(state.PC == 0x06FA + 54);
        }
        SUBCASE("Not taken")
        {
            // ADC #$10 ; BEQ +50
            uint8_t instructions[]{0x69, 0x10, 0xF0, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
        }
    }

    SUBCASE("BIT")
    {
        SUBCASE("Test true")
        {
            // LDA #$CA ; STA $EF ; LDA #$08 ; BIT $EF
            uint8_t instructions[]{0xA9, 0xCA, 0x85, 0xEF, 0xA9, 0x08, 0x24, 0xEF};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 10);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.A == 0x08);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 1);
            CHECK(state.PS.Test<CPU::V>() == 1);
        }
        SUBCASE("Test false")
        {
            // LDA #$23 ; STA $EF ; LDA #$0C ; BIT $EF
            uint8_t instructions[]{0xA9, 0x23, 0x85, 0xEF, 0xA9, 0x0C, 0x24, 0xEF};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 10);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.A == 0x0C);
            CHECK(state.PS.Test<CPU::Z>() == 1);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::V>() == 0);
        }
    }

    SUBCASE("BMI")
    {
        SUBCASE("Taken")
        {
            // LDA #$80 ; BMI +50
            uint8_t instructions[]{0xA9, 0x80, 0x30, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 5);
            CHECK(state.PC == 0x0700 + 54);
        }
        SUBCASE("Taken with page cross")
        {
            // LDA #$80 ; BMI +50
            uint8_t instructions[]{0xA9, 0x80, 0x30, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4, 0x06FA);
            CHECK(state.cycleCount == 6);
            CHECK(state.PC == 0x06FA + 54);
        }
        SUBCASE("Not taken")
        {
            // LDA #$40 ; BMI +50
            uint8_t instructions[]{0xA9, 0x40, 0x30, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
        }
    }

    SUBCASE("BNE")
    {
        SUBCASE("Taken")
        {
            // ADC #$10 ; BNE +50
            uint8_t instructions[]{0x69, 0x10, 0xD0, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 5);
            CHECK(state.PC == 0x0700 + 54);
        }
        SUBCASE("Taken with page cross")
        {
            // ADC #$10 ; BNE +50
            uint8_t instructions[]{0x69, 0x10, 0xD0, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4, 0x06FA);
            CHECK(state.cycleCount == 6);
            CHECK(state.PC == 0x06FA + 54);
        }
        SUBCASE("Not taken")
        {
            // ADC #$00 ; BNE +50
            uint8_t instructions[]{0x69, 0x00, 0xD0, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
        }
    }

    SUBCASE("BPL")
    {
        SUBCASE("Taken")
        {
            // LDA #$40 ; BPL +50
            uint8_t instructions[]{0xA9, 0x40, 0x10, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 5);
            CHECK(state.PC == 0x0700 + 54);
        }
        SUBCASE("Taken with page cross")
        {
            // LDA #$40 ; BPL +50
            uint8_t instructions[]{0xA9, 0x40, 0x10, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4, 0x06FA);
            CHECK(state.cycleCount == 6);
            CHECK(state.PC == 0x06FA + 54);
        }
        SUBCASE("Not taken")
        {
            // LDA #$80 ; BPL +50
            uint8_t instructions[]{0xA9, 0x80, 0x10, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
        }
    }

    SUBCASE("BRK")
    {
        // PC is not actually checked since interrupt vector should be in cartridge space
        uint8_t instructions[]{0x00};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 1);
        CHECK(state.cycleCount == 7);
        CHECK(state.PS.Test<CPU::B>() == 1);
        CHECK(state.PS.Test<CPU::I>() == 1);
    }

    SUBCASE("BVC")
    {
        SUBCASE("Taken")
        {
            // CLV ; BVC +50
            uint8_t instructions[]{0xB8, 0x50, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
            CHECK(state.cycleCount == 5);
            CHECK(state.PC == 0x0700 + 53);
        }
        SUBCASE("Taken with page cross")
        {
            // CLV ; BVC +50
            uint8_t instructions[]{0xB8, 0x50, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3, 0x06FA);
            CHECK(state.cycleCount == 6);
            CHECK(state.PC == 0x06FA + 53);
        }
        SUBCASE("Not taken")
        {
            // LDA #$FF ; STA $EF ; BIT $EF ; BVC +50
            uint8_t instructions[]{0xA9, 0xFF, 0x85, 0xEF, 0x24, 0xEF, 0x50, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 10);
            CHECK(state.PC == 0x0700 + 8);
        }
    }

    SUBCASE("BVS")
    {
        SUBCASE("Taken")
        {
            // LDA #$FF ; STA $EF ; BIT $EF ; BVS +50
            uint8_t instructions[]{0xA9, 0xFF, 0x85, 0xEF, 0x24, 0xEF, 0x70, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 11);
            CHECK(state.PC == 0x0700 + 58);
        }
        SUBCASE("Taken with page cross")
        {
            // LDA #$FF ; STA $EF ; BIT $EF ; BVS +50
            uint8_t instructions[]{0xA9, 0xFF, 0x85, 0xEF, 0x24, 0xEF, 0x70, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8, 0x06F0);
            CHECK(state.cycleCount == 12);
            CHECK(state.PC == 0x06F0 + 58);
        }
        SUBCASE("Not taken")
        {
            // CLV ; BVS +50
            uint8_t instructions[]{0xB8, 0x70, 0x32};
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

    SUBCASE("CLD")
    {
        // CLD
        uint8_t instructions[]{0xD8};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 1);
        CHECK(state.cycleCount == 2);
        CHECK(state.PC == 0x0700 + 1);
        CHECK(state.PS.Test<CPU::D>() == 0);
    }

    SUBCASE("CLI")
    {
        // CLI
        uint8_t instructions[]{0x58};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 1);
        CHECK(state.cycleCount == 2);
        CHECK(state.PC == 0x0700 + 1);
        CHECK(state.PS.Test<CPU::I>() == 0);
    }

    SUBCASE("CLV")
    {
        // CLI
        uint8_t instructions[]{0xB8};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 1);
        CHECK(state.cycleCount == 2);
        CHECK(state.PC == 0x0700 + 1);
        CHECK(state.PS.Test<CPU::V>() == 0);
    }

    SUBCASE("CMP")
    {
        SUBCASE("Positive result")
        {
            // LDA #$4F ; CMP #$1A
            uint8_t instructions[]{0xA9, 0x4F, 0xC9, 0x1A};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.A == 0x4F);
            CHECK(state.PS.Test<CPU::C>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::V>() == 0);
        }
        SUBCASE("Zero result")
        {
            // LDA #$4F ; CMP #$4F
            uint8_t instructions[]{0xA9, 0x4F, 0xC9, 0x4F};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.A == 0x4F);
            CHECK(state.PS.Test<CPU::C>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 1);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("Negative result")
        {
            // LDA #$4F ; CMP #$FA
            uint8_t instructions[]{0xA9, 0x4F, 0xC9, 0xFA};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.A == 0x4F);
            CHECK(state.PS.Test<CPU::C>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
    }

    SUBCASE("CPX")
    {
        SUBCASE("Positive result")
        {
            // LDX #$4F ; CPX #$1A
            uint8_t instructions[]{0xA2, 0x4F, 0xE0, 0x1A};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.X == 0x4F);
            CHECK(state.PS.Test<CPU::C>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::V>() == 0);
        }
        SUBCASE("Zero result")
        {
            // LDX #$4F ; CPX #$4F
            uint8_t instructions[]{0xA2, 0x4F, 0xE0, 0x4F};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.X == 0x4F);
            CHECK(state.PS.Test<CPU::C>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 1);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("Negative result")
        {
            // LDX #$4F ; CPX #$FA
            uint8_t instructions[]{0xA2, 0x4F, 0xE0, 0xFA};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.X == 0x4F);
            CHECK(state.PS.Test<CPU::C>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
    }

    SUBCASE("CPY")
    {
        SUBCASE("Positive result")
        {
            // LDY #$4F ; CPY #$1A
            uint8_t instructions[]{0xA0, 0x4F, 0xC0, 0x1A};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.Y == 0x4F);
            CHECK(state.PS.Test<CPU::C>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::V>() == 0);
        }
        SUBCASE("Zero result")
        {
            // LDY #$4F ; CPY #$4F
            uint8_t instructions[]{0xA0, 0x4F, 0xC0, 0x4F};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.Y == 0x4F);
            CHECK(state.PS.Test<CPU::C>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 1);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("Negative result")
        {
            // LDY #$4F ; CPY #$FA
            uint8_t instructions[]{0xA0, 0x4F, 0xC0, 0xFA};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.Y == 0x4F);
            CHECK(state.PS.Test<CPU::C>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
    }

    SUBCASE("DEC")
    {
        SUBCASE("ZeroPage")
        {
            // LDA #$01 ; STA $EF ; DEC $EF ; LDA $EF
            uint8_t instructions[]{0xA9, 0x01, 0x85, 0xEF, 0xC6, 0xEF, 0xA5, 0xEF};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 13);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.A == 0x00);
            CHECK(state.PS.Test<CPU::Z>() == 1);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("AbsoluteX")
        {
            // LDA #$00 ; LDX #$04 ; STA $03E4 ; DEC $03E0,x ; LDA $03E4
            uint8_t instructions[]{0xA9, 0x00, 0xA2, 0x04, 0x8D, 0xE4, 0x03, 0xDE, 0xE0, 0x03, 0xAD, 0xE4, 0x03};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 13);
            CHECK(state.cycleCount == 19);
            CHECK(state.PC == 0x0700 + 13);
            CHECK(state.A == 0xFF);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 1);
        }
    }

    SUBCASE("DEX")
    {
        // LDX #$01 ; DEX
        uint8_t instructions[]{0xA2, 0x01, 0xCA};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
        CHECK(state.cycleCount == 4);
        CHECK(state.PC == 0x0700 + 3);
        CHECK(state.X == 0x00);
        CHECK(state.PS.Test<CPU::Z>() == 1);
        CHECK(state.PS.Test<CPU::N>() == 0);
    }

    SUBCASE("DEY")
    {
        // LDY #$F0 ; DEY
        uint8_t instructions[]{0xA0, 0xF0, 0x88};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
        CHECK(state.cycleCount == 4);
        CHECK(state.PC == 0x0700 + 3);
        CHECK(state.Y == 0xEF);
        CHECK(state.PS.Test<CPU::Z>() == 0);
        CHECK(state.PS.Test<CPU::N>() == 1);
    }

    SUBCASE("EOR")
    {
        SUBCASE("Result is zero")
        {
            // LDA #$32 ; EOR #$32
            uint8_t instructions[]{0xA9, 0x32, 0x49, 0x32};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x00);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 1);
        }
        SUBCASE("Result is positive")
        {
            // LDA #$32 ; EOR #$4A
            uint8_t instructions[]{0xA9, 0x32, 0x49, 0x4A};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x78);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 0);
        }
        SUBCASE("Result is negative")
        {
            // LDA #$32 ; EOR #$E5
            uint8_t instructions[]{0xA9, 0x32, 0x49, 0xE5};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0xD7);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::N>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 0);
        }
    }

    SUBCASE("INC")
    {
        SUBCASE("ZeroPage")
        {
            // LDA #$FF ; STA $EF ; INC $EF ; LDA $EF
            uint8_t instructions[]{0xA9, 0xFF, 0x85, 0xEF, 0xE6, 0xEF, 0xA5, 0xEF};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.cycleCount == 13);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.A == 0x00);
            CHECK(state.PS.Test<CPU::Z>() == 1);
            CHECK(state.PS.Test<CPU::N>() == 0);
        }
        SUBCASE("AbsoluteX")
        {
            // LDA #$7F ; LDX #$04 ; STA $03E4 ; INC $03E0,x ; LDA $03E4
            uint8_t instructions[]{0xA9, 0x7F, 0xA2, 0x04, 0x8D, 0xE4, 0x03, 0xFE, 0xE0, 0x03, 0xAD, 0xE4, 0x03};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 13);
            CHECK(state.cycleCount == 19);
            CHECK(state.PC == 0x0700 + 13);
            CHECK(state.A == 0x80);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::N>() == 1);
        }
    }

    SUBCASE("INX")
    {
        // LDX #$FF ; INX
        uint8_t instructions[]{0xA2, 0xFF, 0xE8};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
        CHECK(state.cycleCount == 4);
        CHECK(state.PC == 0x0700 + 3);
        CHECK(state.X == 0x00);
        CHECK(state.PS.Test<CPU::Z>() == 1);
        CHECK(state.PS.Test<CPU::N>() == 0);
    }

    SUBCASE("INY")
    {
        // LDY #$F0 ; INY
        uint8_t instructions[]{0xA0, 0xF0, 0xC8};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
        CHECK(state.cycleCount == 4);
        CHECK(state.PC == 0x0700 + 3);
        CHECK(state.Y == 0xF1);
        CHECK(state.PS.Test<CPU::Z>() == 0);
        CHECK(state.PS.Test<CPU::N>() == 1);
    }

    SUBCASE("JMP")
    {
        SUBCASE("Absolute")
        {
            // JMP $07EF
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

    SUBCASE("JSR")
    {
        // JSR $07EF
        uint8_t instructions[]{0x20, 0xEF, 0x07};
        Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
        CHECK(state.cycleCount == 6);
        CHECK(state.PC == 0x07EF);
        CHECK(state.SP == 0xFB);
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

    SUBCASE("LSR")
    {
        SUBCASE("Accumulator")
        {
            // LDA #$B6 ; LSR
            uint8_t instructions[]{0xA9, 0xB6, 0x4A};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 3);
            CHECK(state.A == 0x5B);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 3);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::C>() == 0);
        }
        SUBCASE("ZeroPage")
        {
            // LDA #$63 ; STA $EF ; LSR $EF ; LDX $EF
            uint8_t instructions[]{0xA9, 0x63, 0x85, 0xEF, 0x46, 0xEF, 0xA6, 0xEF};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 8);
            CHECK(state.X == 0x31);
            CHECK(state.cycleCount == 13);
            CHECK(state.PC == 0x0700 + 8);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 0);
            CHECK(state.PS.Test<CPU::C>() == 1);
        }
    }

    SUBCASE("ORA")
    {
        SUBCASE("Result is zero")
        {
            // LDA #$00 ; ORA #$00
            uint8_t instructions[]{0xA9, 0x00, 0x09, 0x00};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x00);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 1);
        }
        SUBCASE("Result is positive")
        {
            // LDA #$32 ; ORA #$4A
            uint8_t instructions[]{0xA9, 0x32, 0x09, 0x4A};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0x7A);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::N>() == 0);
            CHECK(state.PS.Test<CPU::Z>() == 0);
        }
        SUBCASE("Result is negative")
        {
            // LDA #$32 ; ORA #$E5
            uint8_t instructions[]{0xA9, 0x32, 0x09, 0xE5};
            Debugger::CpuState state = testDebugger.ExecuteInstrFromArray(instructions, 4);
            CHECK(state.A == 0xF7);
            CHECK(state.cycleCount == 4);
            CHECK(state.PC == 0x0700 + 4);
            CHECK(state.PS.Test<CPU::N>() == 1);
            CHECK(state.PS.Test<CPU::Z>() == 0);
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