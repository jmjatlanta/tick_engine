#include <gtest/gtest.h>
#include <fstream>
#include <chrono>
#include <filesystem>
#include "TickWriter.h"
#include "TickReader.h"
#include "TickCoordinator.h"

using namespace tick_engine;

TEST(TickCoordinatorTest, basics)
{
    if (std::filesystem::exists("test1.out"))
        std::filesystem::remove("test1.out");
    if (std::filesystem::exists("test2.out"))
        std::filesystem::remove("test2.out");

    auto point = std::chrono::system_clock::now();
    uint64_t origStartTimeNs = duration_cast<std::chrono::nanoseconds>(point.time_since_epoch()).count(); 
    uint64_t origStartTimeS = duration_cast<std::chrono::seconds>(point.time_since_epoch()).count(); 

    {
        // write the test files
        std::ofstream out("test1.out", std::ios_base::binary);
        TickHeader header;
        header.version = 0x01;
        std::string contract{"123456789"};
        contract.copy(header.contract, 10);
        header.contract[9] = 0;
        header.startTime = origStartTimeS;
        TickWriter writer(out, header);
        Tick tick;
        tick.price = 10.01;
        tick.type = 'T';
        tick.time = origStartTimeNs;
        tick.size = 1;
        writer.write(tick);
    }
    {
        std::ofstream out("test2.out", std::ios_base::binary);
        TickHeader header;
        header.version = 0x01;
        std::string contract{"987654321"};
        contract.copy(header.contract, 10);
        header.contract[9] = 0;
        header.startTime = origStartTimeS;
        TickWriter writer(out, header);
        Tick tick;
        tick.price = 10.02;
        tick.type = 'T';
        tick.time = origStartTimeNs + 1;
        tick.size = 1;
        writer.write(tick);
    }
    ASSERT_TRUE( std::filesystem::exists("test1.out"));
    ASSERT_TRUE( std::filesystem::exists("test2.out"));
    {
        // read the file
        std::ifstream in1("test1.out", std::ios_base::binary);
        std::shared_ptr<TickReader> reader1 = std::make_shared<TickReader>(in1);
        std::ifstream in2("test2.out", std::ios_base::binary);
        std::shared_ptr<TickReader> reader2 = std::make_shared<TickReader>(in2);
        TickCoordinator coordinator;
        coordinator.AddTickReader(reader1.get());
        coordinator.AddTickReader(reader2.get());
        class MyTickHandler : public TickHandler
        {
            public:
            void OnTick(const std::string_view contract, const Tick& tick) override
            {
                lastContract = contract;
                lastTick = tick;
            }
            std::string lastContract;
            Tick lastTick;
        };
        MyTickHandler myTickHandler;
        coordinator.AddTickHandler(&myTickHandler); 

        coordinator.step();
        EXPECT_EQ(myTickHandler.lastContract, "123456789");
        EXPECT_EQ(myTickHandler.lastTick.price, 10.01f);
        coordinator.step();
        EXPECT_EQ(myTickHandler.lastContract, "987654321");
        EXPECT_EQ(myTickHandler.lastTick.price, 10.02f);
        coordinator.RemoveTickReader(reader1.get());
        coordinator.RemoveTickReader(reader2.get());
        coordinator.RemoveTickHandler(&myTickHandler);
    }

    if (std::filesystem::exists("test1.out"))
        std::filesystem::remove("test1.out");
    if (std::filesystem::exists("test2.out"))
        std::filesystem::remove("test2.out");
}

