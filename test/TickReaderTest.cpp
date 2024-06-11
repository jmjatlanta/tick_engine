#include <gtest/gtest.h>
#include "TickReader.h"

TEST(TickReaderTest, basic)
{
    std::ifstream input("test.tick");
    TickReader reader(input);
}
