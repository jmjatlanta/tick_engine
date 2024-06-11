#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <string_view>

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

class BarBuilder
{
    public:
    BarBuilder(const std::string_view contract);

    void AddHandler(std::shared_ptr<BarHandler> in, BarDuration duration);
    void RemoveHandler(std::shared_ptr<BarHandler> in);

    protected:
    std::vector<std::shared_ptr<BarHandler>> handlers;
};
