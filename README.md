## A ticker plant and OHLC bar generator

These are some basic tools primarily built to backtest strategies. The idea is to read historical tick 
files and generate events that mimic what would be received from an exchange or a brokerage. The tools
include:

- TickReader - Read the tick records from storage
- TickCoordinator - Allow several TickReaders and re-send their events in the correct order
- BarBuilder - Receive tick events (from TickCoordinator) and build historical bars based on the ticks

NOTE: capturing ticks from brokers/exchanges is a necessary part, but not part of this library. The goal
is to keep it broker/exchange agnostic. TickWriter and TickReader can help with such a task.

Dependencies:
- C++20 for filesystem and chrono, C++17 may work, untested
- CMake for building
- Google GTest was used for tests (see `test` directory)
- Built on Ubuntu and sometimes MacOS, but should be platform agnostic. Adjustment of CMake files may be necessary for other
platforms.

TODOs:
- Times are stored in 32-bit and 64-bit values (second and nanosecond durations). While storage is probably best 
with such values, it would probably be better to use chrono soon after reading from disk. This makes any needed
manipulation by the layers above easier.
- There is a mix of shared_ptr and raw pointer for subscribing/unsubscribing. This should be standardized. Raw
pointer is best in this case IMO.
- tick_engine is a poor choice. It is more of a ticker plant / bar plant.
