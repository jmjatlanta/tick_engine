#pragma once
#include <fstream>
#include "Tick.h"

namespace tick_engine
{

class TickWriter
{
    public:
    TickWriter(std::ofstream& stream, const TickHeader& header);

    void write(const Tick& in);

    protected:
    std::ofstream& out_;
};

} // namespace tick_engine

