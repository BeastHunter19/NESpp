#ifndef EMULATOR_H
#define EMULATOR_H

#include "EmulatorCore.h"

/*
 * Main emulator class; works as a high level interface to the
 * NES core emulator. Only exposes functionality needed by an
 * application to run games.
 */

class Emulator : public EmulatorCore
{
public:
    Emulator();
    ~Emulator() = default;

    void Start() { core->ResetRAM(); }
};

#endif // EMULATOR_H