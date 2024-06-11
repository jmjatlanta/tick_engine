## A tick generator

These are some basic tools primarily built to backtest strategies. The idea is to read historical tick 
files and generate events that mimic what would be received from an exchange or a brokerage. The tools
include:

- TickReader - Read the tick records from storage
- TickCoordinator - Allow several TickReaders and re-send their events in the correct order
- BarBuilder - Receive tick events (from TickCoordinator) and build historical bars based on the ticks

NOTE: capturing ticks from brokers/exchanges is a necessary part, but not part of this library. The goal
is to keep it broker/exchange agnostic.

Dependencies:
- C++20 for filesystem and chrono, C++17 may work, untested
- CMake for building
- Google GTest was used for tests (see `test` directory)
- Built on Ubuntu and sometimes MacOS, but should be platform agnostic. Adjustment of CMake files may be necessary for other
platforms.

