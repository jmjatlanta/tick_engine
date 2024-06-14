#include <gtest/gtest.h>
#include "TickReader.h"

TEST(TickReaderTest, basic)
{
    std::ifstream input("test.tick");
    tick_engine::TickReader reader(input);
}
