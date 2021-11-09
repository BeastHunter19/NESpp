#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "NES.h"

/*
 * Alternative frontend for the NES emulator core.
 * Exposes more low level functionality and is
 * declared as friend by most components, allowing
 * to create a debugger for the console.
 */

class Debugger : private NES
{
public:
    Debugger();
    ~Debugger() = default;

    // Allows executing the given number of instructions
    void ExecuteInstructions(int numberToExecute);

    const std::array<uint8_t, 2048>& GetMemoryState() const;

private:
};

#endif // DEBUGGER_H