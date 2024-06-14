#include "BarBuilder.h"
#include <algorithm>

namespace tick_engine
{

BarBuilder::BarBuilder(TickCoordinator* coordinator)
    : tickCoordinator(coordinator)
{
    tickCoordinator->AddTickHandler(this);
}

void BarBuilder::AddHandler(BarHandler* in)
{
    handlers.push_back(in);
}

void BarBuilder::RemoveHandler(BarHandler* in)
{
    auto itr = std::find_if(handlers.begin(), handlers.end(), [in](BarHandler* curr)
            { return curr == in; });
    if (itr != handlers.end())
        handlers.erase(itr);
}

void BarBuilder::AddDuration(const std::string_view contract, BarDuration duration)
{
    // find the contract in the map
    std::string cont{contract};
    auto& vec = latestBars[cont];
    // if we don't already have an entry for that duration, add it
    auto itr = std::find_if(vec.begin(), vec.end(), [duration](DurationAndBar& curr)
            { return curr.duration == duration; });
    if (itr == vec.end())
        vec.push_back( DurationAndBar{duration, Bar{}});
}

uint64_t to_nanoseconds(const BarDuration& duration)
{
    uint64_t secs = 0;
    switch(duration)
    {
        case BarDuration::SEC_30:
            secs = 30;
            break;
        case BarDuration::MIN_1:
            secs = 60;
            break;
        case BarDuration::MIN_2:
            secs = 60 * 2;
            break;
        case BarDuration::MIN_5:
            secs = 60 * 5;
            break;
        case BarDuration::MIN_10:
            secs = 60 * 10;
            break;
        case BarDuration::MIN_15:
            secs = 60 * 15;
            break;
        case BarDuration::MIN_30:
            secs = 60 * 30;
            break;
        case BarDuration::HOUR_1:
            secs = 60 * 60;
            break;
        case BarDuration::HOUR_4:
            secs = 60 * 60 * 4;
            break;
        case BarDuration::DAY:
            secs = 60 * 60 * 24;
            break;
        case BarDuration::WEEK:
            secs = 60 * 60 * 24 * 7;
            break;
        case BarDuration::MONTH: // TODO: Fix these
            secs = 60 * 60 * 24 * 30;
            break;
        case BarDuration::YEAR:
            secs = 60 * 60 * 24 * 365;
            break;
    }
    return secs * 1000000000;
}

bool withinRange(const BarDuration& duration, const Bar& bar, uint64_t time)
{
    // does this time fit in the current bar?
    return bar.time + to_nanoseconds(duration) >= time;
}

uint64_t calculateStartTime(uint64_t time, BarDuration duration)
{
    // given the time and duration, calculate the time of the 
    // bar where this time belongs.
    // in other words, round down to the nearest duration
    uint64_t length = to_nanoseconds(duration);
    return (time / length) * length;
}

void BarBuilder::OnTick(std::string_view contract, const Tick& tick)
{
    if (tick.type != 'T') // only process ticks
        return;

    // update all bars related to this contract, sending notifications if necessary
    std::string cont{contract};
    auto& vec = latestBars[cont];
    std::for_each(vec.begin(), vec.end(), [tick, contract, this](DurationAndBar& db)
            {
                bool somethingChanged = false;
                if (withinRange(db.duration, db.bar, tick.time))
                {
                    // update if we need to
                    if (tick.size > 0)
                    {
                        somethingChanged = true;
                        db.bar.volume += tick.size;
                        db.bar.close = tick.price;
                        if (db.bar.close < db.bar.low)
                            db.bar.low = db.bar.close;
                        if (db.bar.close > db.bar.high)
                            db.bar.high = db.bar.close;
                    }
                }
                else
                {
                    // tick outside range, create new bar
                    somethingChanged = true;
                    db.bar.time = calculateStartTime(tick.time, db.duration);
                    db.bar.close = tick.price;
                    db.bar.open  = tick.price;
                    db.bar.high  = tick.price;
                    db.bar.low   = tick.price;
                    db.bar.volume = tick.size;
                }
                // if we did anything, notify listeners
                if (somethingChanged)
                {
                    std::for_each(handlers.begin(), handlers.end(), 
                            [db, contract](BarHandler* curr)
                            { curr->OnBar(contract, db.duration, db.bar); });
                }
            });
}

} // namespace tick_engine
