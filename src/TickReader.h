#pragma once
#include <fstream>
#include "Tick.h"


/***
 * Read tick files
 */

namespace tick_engine
{

class TickReader
{
    public:
    TickReader(std::ifstream& stream);

    /***
     * @returns the next Tick from the file
     */
    Tick next();
    /***
     * @returns TRUE if eof has been reached
     */
    bool eof();

    /***
     * @returns the header information from the file
     */
    TickHeader header() const;

    protected:
    std::ifstream& in_;
    TickHeader header_;
};

} // namespace tick_engine
