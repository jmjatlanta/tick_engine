#include <cstring>
#include <vector>
#include "signal.h" // ctrl-c
#include "TickWriter.h"
#include "Contract.h"
#include "IBConnector.hpp"
#include "ContractBuilder.hpp"
bool ctrl_c_pressed = false;

void my_handler(int s)
{
    ctrl_c_pressed = true;
}

class TickWatcher : public ib_helper::TickHandler
{
    public:
    TickWatcher(const ContractDetails& dets, std::shared_ptr<ib_helper::IBConnector> conn) 
        : out( std::ofstream(std::string(dets.contract.symbol + ".out").c_str(), std::ios_base::binary))
          , tickWriter(out, TickHeader{})
          , conn(conn)
    {
        //contract, this, tickType, numberOfTicks, ignoreSize
        reqId = conn->SubscribeToTickByTick(dets.contract, this, "AllLast", 0, 0);
    }
    ~TickWatcher()
    {
        if (reqId != -1 && conn != nullptr)
            conn->UnsubscribeFromTickByTick(reqId);
    }
    virtual void OnTickPrice(int tickerId, int field, double price, TickAttrib attribs) override {}
    virtual void OnTickSize(int tickerId, int field, Decimal size) override {}
    virtual void OnTickOptionComputation(int tickerId, int field, int tickAttrib, double impliedVol, double delta, 
            double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice) override {}
    virtual void OnTickGeneric(int tickerId, int tickType, double value) override {}
    virtual void OnTickString(int tickerId, int tickType, const std::string& value) override {}
    virtual void OnTickEFP(int tickerId, int tickType, double basisPoints, const std::string& formattedBasisPoints, 
            double impliedFuture, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, 
            double dividendsToLastTradeDate) override {}
    /***
     * Used with reqTickByTick
     * @param reqId
     * @param tickType
     * @param time
     * @param price
     * @param size
     * @param tickAttribLast
     * @param exchange
     * @param specialConditions
     */
    virtual void OnTickLast(int reqId, int tickType, long time, double price, Decimal size, TickAttribLast tickAttribLast,
            const std::string& exchange, const std::string&  specialConditions) override {}
    virtual void OnTickByTickAllLast(int reqId, int tickType, time_t time, double price, Decimal size,
            const TickAttribLast& tickAttribLast, const std::string& exchange, 
            const std::string& specialConditions) override 
    {
        Tick t;
        t.size = decimalToDouble(size);
        t.time = time * 1000000000;
        t.price = price;
        t.type = 'T';
        tickWriter.write(t);
    }
    virtual void OnTickByTickBidAsk(int reqId, time_t time, double bidPrice, double askPrice, Decimal bidSize,
            Decimal askSize, const TickAttribBidAsk& tickAttribBidAsk) override {}
    virtual void OnTickByTickMidPoint(int reqId, time_t time, double midPoint) override {}

    private:
    std::shared_ptr<ib_helper::IBConnector> conn;
    std::ofstream out;
    TickWriter tickWriter;
    int reqId = -1;
};

int main(int argc, char** argv)
{
    if (argc == 1)
        exit(1);

    // ctrl+c
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);

    std::string host = "localhost";
    uint32_t port = 7497;
    uint32_t clientId = 10;

    std::shared_ptr<ib_helper::IBConnector> conn = std::make_shared<ib_helper::IBConnector>(host, port, clientId);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (!conn->IsConnected())
        exit(1);

    // parse the command line arguments and build TickWatchers for each
    std::vector<std::shared_ptr<TickWatcher>> coll;
    for(int i = 1; i < argc; ++i)
    {
        std::string wholeTicker{argv[i]};
        auto pos = wholeTicker.find('.');
        if (pos == std::string::npos)
            exit(1);
        auto type = wholeTicker.substr(0, pos);
        auto ticker = wholeTicker.substr(pos+1);
        ib_helper::ContractBuilder builder(conn.get());
        ContractDetails dets;
        if (type == "STK")
            dets = builder.BuildStock(ticker);
        else
            dets = builder.BuildFuture(ticker);
        std::cout << "Starting tick watcher for " << dets.contract.symbol << std::endl;
        coll.emplace_back( std::make_shared<TickWatcher>( dets, conn ));
    }
    // now wait forever
    while(!ctrl_c_pressed) { std::this_thread::sleep_for(std::chrono::seconds(1)); }
    std::cout << "Shutdown after CTRL+C\n";
    return 0;
}

