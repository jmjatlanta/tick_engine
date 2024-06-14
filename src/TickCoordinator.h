#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "TickReader.h"

namespace tick_engine
{
/***
 * Those that wish to receive ticks should implement this interface
 */
class TickHandler
{
    public:
    /***
     * A tick happened
     * @param contract where the tick came from
     * @param tick the financial information
     */
    virtual void OnTick(std::string_view contract, const Tick& tick) = 0;
};

class TickCoordinator
{
    public:
    TickCoordinator();
    ~TickCoordinator();

    /***
     * Add a reader to the collection
     * @param in the reader
     */
    void AddTickReader(TickReader* in);
    void RemoveTickReader(TickReader* in);

    /****
     * @param in the tick handler
     */
    void AddTickHandler(TickHandler* in);
    void RemoveTickHandler(TickHandler* in);

    /***
     * @brief progress 1 tick
     * @returns true on success
     */
    bool step();

    protected:
    std::vector<TickReader*> readers;
    std::vector<TickHandler*> tickHandlers;
    std::unordered_map<TickReader*, Tick> latestTicks;
};

} // namespace tick_engine
