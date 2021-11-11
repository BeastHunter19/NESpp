#ifndef EMULATORCORE_H
#define EMULATORCORE_H

#include "NES.h"
#include <memory>
class EmulatorCore
{
public:
    EmulatorCore() { core = std::make_shared<NES>(); }
    EmulatorCore(const EmulatorCore& other) { core = other.core; }
    EmulatorCore& operator=(const EmulatorCore& other)
    {
        core = other.core;
        return *this;
    }
    virtual ~EmulatorCore() = default;

protected:
    std::shared_ptr<NES> core;
};

#endif // EMULATORCORE_H