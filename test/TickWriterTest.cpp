#include <gtest/gtest.h>
#include <fstream>
#include <chrono>
#include <filesystem>
#include "TickWriter.h"
#include "TickReader.h"

using namespace tick_engine;

TEST(TickWriterTest, basics)
{
    if (std::filesystem::exists("test.out"))
        std::filesystem::remove("test.out");

    auto point = std::chrono::system_clock::now();
    uint64_t origStartTimeNs = duration_cast<std::chrono::nanoseconds>(point.time_since_epoch()).count(); 
    uint64_t origStartTimeS = duration_cast<std::chrono::seconds>(point.time_since_epoch()).count(); 

    {
        // write the test file
        std::ofstream out("test.out", std::ios_base::binary);
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
    ASSERT_TRUE( std::filesystem::exists("test.out"));
    {
        // read the file
        std::ifstream in("test.out", std::ios_base::binary);
        TickReader reader(in);
        // check the header
        TickHeader header = reader.header();
        EXPECT_EQ(header.version, 0x01);
        EXPECT_EQ(std::string(header.contract), "123456789");
        EXPECT_EQ(header.startTime, origStartTimeS);
        // check the record
        EXPECT_FALSE(reader.eof());
        Tick record = reader.next();
        EXPECT_EQ(record.type, 'T');
        EXPECT_EQ(record.size, 1);
        EXPECT_EQ(record.time, origStartTimeNs);
        EXPECT_EQ(record.price, 10.01f);
        // check eof
        EXPECT_TRUE(reader.eof());
    }

    if (std::filesystem::exists("test.out"))
        std::filesystem::remove("test.out");
}
