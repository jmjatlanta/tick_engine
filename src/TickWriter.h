#pragma once
#include <fstream>
#include "Tick.h"

class TickWriter
{
    public:
    TickWriter(std::ofstream& stream, const TickHeader& header);

    void write(const Tick& in);

    protected:
    std::ofstream& out_;
};

