#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <string_view>
#include <string>
#include <unordered_map>
#include "TickCoordinator.h"

namespace tick_engine
{

enum class BarDuration
{
    SEC_30,
    MIN_1,
    MIN_2,
    MIN_5,
    MIN_10,
    MIN_15,
    MIN_30,
    HOUR_1,
    HOUR_4,
    DAY,
    WEEK,
    MONTH,
    YEAR
};

class Bar
{
    public:
    float open;
    float high;
    float low;
    float close;
    uint32_t volume;
    uint64_t time; // time in nanoseconds
};

class BarHandler
{
public:
    virtual void OnBar(const std::string_view contract, BarDuration duration, const Bar bar) = 0;
};

/***
 * A simplistic bar builder. It listens for ticks, and builds bars, firing events when a bar
 * is updated
 */
class BarBuilder : public TickHandler
{
    public:
    BarBuilder(TickCoordinator* coordinator);

    /***
    * Add a new listener
    * @param in the listener
    */
    void AddHandler(BarHandler* in);
    void RemoveHandler(BarHandler* in);

    /****
    * Start sending bars of a particular contract/duration
    * @param contract the contract
    * @param duration the duration
    */
    void AddDuration(const std::string_view contract, BarDuration duration);

    // implementation of TickHandler interface
    virtual void OnTick(std::string_view contract, const Tick& tick) override;

    protected:
    struct DurationAndBar
    {
        BarDuration duration;
        Bar bar;
    };

    std::vector<BarHandler*> handlers; // all receive all messages
    std::unordered_map<std::string, std::vector<DurationAndBar>> latestBars; // contract to collection of bars, 1 per durration
    TickCoordinator* tickCoordinator;
};

} // namespace tick_engine
