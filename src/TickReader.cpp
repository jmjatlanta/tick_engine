#include "TickReader.h"

TickReader::TickReader(std::ifstream& stream) : in_(stream)
{
    // read the header
    in_.read((char*)&header_, sizeof(TickHeader));
}

Tick TickReader::next()
{
    Tick curr;
    in_.read((char*)&curr, sizeof(Tick));
    return curr;
}

bool TickReader::eof()
{
    return in_.peek() == EOF;
}

TickHeader TickReader::header() const { return header_; }

