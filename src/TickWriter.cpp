#include "TickWriter.h"

TickWriter::TickWriter(std::ofstream& stream, const TickHeader& header)
    : out_(stream)
{
    out_.write((char*)&header, sizeof(TickHeader));
}

void TickWriter::write(const Tick& in)
{
    out_.write((char*)&in, sizeof(Tick));
}

