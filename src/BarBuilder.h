#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <string_view>
#include <string>
#include <unordered_map>

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
class BarBuilder
{
    public:
    BarBuilder(const std::string_view contract);

    /***
    * Add a new listener
    * @param in the listener
    */
    void AddHandler(std::shared_ptr<BarHandler> in);
    void RemoveHandler(std::shared_ptr<BarHandler> in);
    /****
    * Start sending bars of a particular contract/duration
    * @param contract the contract
    * @param duration the duration
    */
    void AddDuration(const std::string_view contract, BarDuration duration);

    protected:
    std::vector<std::shared_ptr<BarHandler>> handlers; // all receive all messages
    std::unordered_map<std::string, std::vector<Bar>> latestBars; // contract to collection of bars, 1 per durration
};
