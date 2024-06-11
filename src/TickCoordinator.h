#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "TickReader.h"

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

    /***
     * Add a reader to the collection
     * @param in the reader
     */
    void AddTickReader(std::shared_ptr<TickReader> in);

    /****
     * @param in the tick handler
     */
    void AddTickHandler(std::shared_ptr<TickHandler> in);

    /***
     * @brief progress 1 tick
     */
    void step();

    protected:
    std::vector<std::shared_ptr<TickReader>> readers;
    std::vector<std::shared_ptr<TickHandler>> tickHandlers;
    std::unordered_map<std::shared_ptr<TickReader>, Tick> latestTicks;
};

