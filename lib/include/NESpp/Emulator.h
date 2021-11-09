#ifndef EMULATOR_H
#define EMULATOR_H

#include "NES.h"

/*
 * Main emulator class; works as a high level interface to the
 * NES core emulator. Only exposes functionality needed by an
 * application to run games.
 */

class Emulator : private NES
{
public:
    Emulator();
    ~Emulator();

private:
};

#endif // EMULATOR_H