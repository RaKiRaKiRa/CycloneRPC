/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-11-23 21:10
 * Last modified : 2019-12-06 03:10
 * Filename      : 
 * Description   : 
 **********************************************************/
#include "ArithmeticServiceStub.h"
#include <CycloneRpc/RpcServer.h>
#include <Cyclone/net/EventLoopThread.h>


class ArithmeticService: public ArithmeticServiceStub<ArithmeticService>
{
public:
    ArithmeticService(RpcServer & server):
        ArithmeticServiceStub(server),
        loopThread_(new EventLoopThread),
        loop_(loopThread_->start())
    {
        
    }

    void Add(double lhs, double rhs, const UserDoneCallback& cb)
    {
        loop_->queueInLoop(std::bind(cb, json::Value(lhs + rhs)));
    }
    void Sub(double lhs, double rhs, const UserDoneCallback& cb)
    {
        loop_->queueInLoop(std::bind(cb, json::Value(lhs - rhs)));
    }
    void Mul(double lhs, double rhs, const UserDoneCallback& cb)
    {
        loop_->queueInLoop(std::bind(cb, json::Value(lhs * rhs)));
    }
    void Div(double lhs, double rhs, const UserDoneCallback& cb)
    {
        loop_->queueInLoop(std::bind(cb, json::Value(lhs / rhs)));
    }
private:
    EventLoopThread* loopThread_;
    EventLoop* loop_;
};

int main()
{
    EventLoop loop;
    sockaddr_in addr = fromPort(4396);
    RpcServer rpcServer(&loop, addr);
    ArithmeticService service(rpcServer);

    rpcServer.start();
    loop.loop();
}