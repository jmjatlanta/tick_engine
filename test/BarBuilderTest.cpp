#include <gtest/gtest.h>
#include <fstream>
#include "BarBuilder.h"
#include "TickCoordinator.h"
#include "TickWriter.h"

using namespace tick_engine;

void buildTickFile(std::string_view filename)
{
    std::ofstream outFile(filename.data(), std::ios_base::binary);
    TickHeader header;
    strcpy(header.contract, "MSFT");
    header.version = 0x01;
    header.startTime = 1718203162; // 6/12/2024 14:39:22 GMT
    TickWriter writer(outFile, header);
    uint64_t currTime = header.startTime;

    tick_engine::Tick tick;
    tick.type = 'T';
    tick.time = currTime * 1000000000;
    uint8_t factor = 1;
    for(int i = 0; i < 1000000; ++i)
    {
        if (i % 10 == 0)
            factor *= -1;

        tick.size = 1;
        tick.time += 1000000000; // 1 second more
        tick.price += 0.01 * factor;
        writer.write(tick);
    }
}

TEST(BarBuilderTest, basics)
{
    class MyBarHandler : public BarHandler
    {
        public:
        void OnBar(const std::string_view contract, BarDuration duration, const Bar bar) override
        {
            switch(duration)
            {
                case BarDuration::MIN_5:
                    updateBar(bars_5min, bar);
                    break;
                case BarDuration::MIN_1:
                    updateBar(bars_1min, bar);
                    break;
                default:
                    break;
            }
        }
        std::vector<Bar> bars_5min;
        std::vector<Bar> bars_1min;
        private:
        void updateBar(std::vector<Bar>& bars, const Bar& bar)
        {
            if (bars.size() == 0 || bars[bars.size()-1].time != bar.time)
            {
                // add it to the last
                bars.push_back(bar);
            }
            else
            {
                // update the last
                Bar& curr = bars[bars.size()-1];
                curr.open = bar.open;
                curr.low = bar.low;
                curr.high = bar.high;
                curr.close = bar.close;
                curr.volume = bar.volume;
            }
        }
    };

    buildTickFile("ticks.out");
    std::ifstream ticks("ticks.out", std::ios_base::binary);
    std::shared_ptr<TickReader> tickReader = std::make_shared<TickReader>( ticks );
    TickCoordinator coordinator;
    coordinator.AddTickReader( tickReader.get() );

    BarBuilder barBuilder(&coordinator); // NOTE: it registers itself as a handler
    MyBarHandler barHandler;
    barBuilder.AddHandler(&barHandler);
    barBuilder.AddDuration("MSFT", BarDuration::MIN_1);
    barBuilder.AddDuration("MSFT", BarDuration::MIN_5);

    // feed the BarBuilder a bunch of ticks
    while (coordinator.step()) { /* Continue util the end */ }

    // now we have a bunch of bars
    EXPECT_EQ(barHandler.bars_5min.size(), 3335);
    EXPECT_EQ(barHandler.bars_1min.size(), 16668);
}
