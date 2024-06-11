#include <algorithm>
#include <limits>
#include "TickCoordinator.h"

TickCoordinator::TickCoordinator()
{
}

void TickCoordinator::AddTickReader(std::shared_ptr<TickReader> in)
{
    readers.push_back(in);
}

void TickCoordinator::AddTickHandler(std::shared_ptr<TickHandler> in)
{
    tickHandlers.push_back(in);
}

std::pair<std::shared_ptr<TickReader>, Tick> getLatest(std::unordered_map<std::shared_ptr<TickReader>, Tick>&readers)
{
    // go through the readers, picking the oldest
    std::shared_ptr<TickReader> oldestReader = nullptr;
    Tick oldestTick;
    oldestTick.time = std::numeric_limits<uint64_t>::max();
    std::for_each(readers.begin(), readers.end(), [&oldestTick, &oldestReader]( std::pair<std::shared_ptr<TickReader>, Tick>& curr)
            { 
                if ( curr.second.time < oldestTick.time)
                {
                    oldestReader = curr.first;
                    oldestTick = curr.second;
                }
                });
    return std::pair<std::shared_ptr<TickReader>, Tick>(oldestReader, oldestTick);
}

void TickCoordinator::step()
{
    // get the next step
    std::pair<std::shared_ptr<TickReader>, Tick> latestPair = getLatest(latestTicks);
    Tick t = latestPair.second;
    // advance the collection
    latestTicks[latestPair.first] = latestPair.first->next();
    // notify the handlers
    std::string contract = latestPair.first->header().contract;
    std::for_each(tickHandlers.begin(), tickHandlers.end(), [t, contract](std::shared_ptr<TickHandler> curr)
            { curr->OnTick(contract, t); });
}
