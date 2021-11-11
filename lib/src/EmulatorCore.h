#ifndef EMULATORCORE_H
#define EMULATORCORE_H

#include "MainBus.h"
#include <memory>
class EmulatorCore
{
public:
    EmulatorCore() { core = std::make_shared<MainBus>(); }
    EmulatorCore(const EmulatorCore& other) { core = other.core; }
    ~EmulatorCore() = default;

protected:
    std::shared_ptr<MainBus> core;
};

#endif // EMULATORCORE_H