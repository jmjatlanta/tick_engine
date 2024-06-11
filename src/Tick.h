#pragma once
#include <cstdint>
#include <string>

/****
 * These are the low-level structs for reading/writing Tick data to/from the disk
 */


#pragma pack(1)
/***
 * How a Tick could be stored on disk. This one focuses on space saving.
 */
class Tick
{
    public:
    char type; // (B)id, (A)sk, (T)ick
    uint64_t time; // time in nanoseconds
    float price; // not accurate enough for crypto, but good for most else
    uint32_t size; // the volume

};

/*****
 * Note that having a file header is a good idea. Versioning, contract, date of
 * capture, etc. are good things to have at the top of your file
 */
class TickHeader
{
    public:
    uint8_t version; // helps control the format of the file
    char contract[10]; // some kind of contract identifier
    uint32_t startTime; // time as unix epoch of when these samples were taken
};
#pragma pack(8)

std::string to_string(const Tick& in);
std::string to_string(const TickHeader& in);
