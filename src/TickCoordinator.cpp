#include <algorithm>
#include <limits>
#include "TickCoordinator.h"

TickCoordinator::TickCoordinator()
{
}

TickCoordinator::~TickCoordinator()
{
    latestTicks.clear();
    readers.clear();
    tickHandlers.clear();
}

void TickCoordinator::AddTickReader(std::shared_ptr<TickReader> in)
{
    readers.push_back(in);
    if (!in->eof())
    {
        latestTicks[in] = in->next();
    }
}
void TickCoordinator::RemoveTickReader(std::shared_ptr<TickReader> in)
{
    for(auto itr = readers.begin(); itr != readers.end(); ++itr)
    {
        if (in.get() == (*itr).get())
        {
            readers.erase(itr);
            break;
        }
    }
    for(auto itr = latestTicks.begin(); itr != latestTicks.end(); ++itr)
    {
        if ( in.get() == (*itr).first.get())
        {
            latestTicks.erase(itr);
            break;
        }
    }
}
void TickCoordinator::AddTickHandler(TickHandler* in)
{
    tickHandlers.push_back(in);
}
void TickCoordinator::RemoveTickHandler(TickHandler* in)
{
    for(auto itr = tickHandlers.begin(); itr != tickHandlers.end(); ++itr)
    {
        if (in == (*itr))
        {
            tickHandlers.erase(itr);
            break;
        }
    }
}

std::pair<std::shared_ptr<TickReader>, Tick> getLatest(std::unordered_map<std::shared_ptr<TickReader>, Tick>&readers)
{
    // go through the readers, picking the oldest
    std::shared_ptr<TickReader> oldestReader = nullptr;
    Tick oldestTick;
    oldestTick.time = std::numeric_limits<uint64_t>::max();
    std::for_each(readers.begin(), readers.end(), 
                  [&oldestTick, &oldestReader]( std::pair<const std::shared_ptr<TickReader>, Tick>& curr)
            { 
                if ( curr.second.time < oldestTick.time)
                {
                    oldestReader = curr.first;
                    oldestTick = curr.second;
                }
                });
    return std::pair<std::shared_ptr<TickReader>, Tick>(oldestReader, oldestTick);
}

bool TickCoordinator::step()
{
    if (latestTicks.size() == 0)
        return false;

    // get the next step
    std::pair<std::shared_ptr<TickReader>, Tick> latestPair = getLatest(latestTicks);
    Tick t = latestPair.second;
    // advance the collection
    if (latestPair.first->eof())
    {
        // remove from latestPair
        for(auto itr = latestTicks.begin(); itr != latestTicks.end(); ++itr)
        {
            if (latestPair.first.get() == (*itr).first.get())
            {
                latestTicks.erase(itr);
                break;
            }
        }
    }
    else
    {
        // advance to the next in the file
        latestTicks[latestPair.first] = latestPair.first->next();
    }
    // notify the handlers
    std::string contract = latestPair.first->header().contract;
    std::for_each(tickHandlers.begin(), tickHandlers.end(), [t, contract](TickHandler* curr)
            { curr->OnTick(contract, t); });
    return true;
}

