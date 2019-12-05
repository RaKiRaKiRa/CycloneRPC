
#ifndef CYCLONERPC_ARITHMETICCLIENTSTUB_H
#define CYCLONERPC_ARITHMETICCLIENTSTUB_H
#include <CycloneRpc/RpcClient.h>
#include <jackson/jackson/Value.h>
#include <Cyclone/net/base/Logging.h>
class ArithmeticClientStub: noncopyable
{
public:
    ArithmeticClientStub(EventLoop* loop, sockaddr_in& addr, const std::string& name = "ArithmeticClientStub"):
        client_(loop, addr, name)
    {
        client_.setConnCallback(std::bind(&ArithmeticClientStub::onConnection, this, _1));
    }

    void onConnection(const ConnectionPtr& conn)
    {
        LOG_INFO << toIpPort(conn->peer() ) << (conn->connected() ? "UP" : "DOWN");
        MutexGuard lock(mutex_);
        if(conn->connected())
        {
            conn_ = conn;
            if(cb_)
            {
                cb_(conn_);
            }
        }
        else
        {
            if(cb_)
            {
                cb_(conn_);
            }
            conn_.reset();
        }
    }

    void setConnCallback(ConnCallback cb)
    {
        cb_ = std::move(cb);
    }

    void start()
    {
        client_.start();
    }


    void Add(double lhs, double rhs,  const ResponseCallback& cb)
    {
        json::Value params(json::TYPE_OBJECT);

        params.addMember("lhs", lhs);
    
        params.addMember("rhs", rhs);
    
        json::Value call(json::TYPE_OBJECT);
        call.addMember("jsonrpc", "2.0");
        call.addMember("method", "Arithmetic.Add");
        call.addMember("params", params);
        client_.Call(conn_, call, cb);
    }
    
    void Sub(double lhs, double rhs,  const ResponseCallback& cb)
    {
        json::Value params(json::TYPE_OBJECT);

        params.addMember("lhs", lhs);
    
        params.addMember("rhs", rhs);
    
        json::Value call(json::TYPE_OBJECT);
        call.addMember("jsonrpc", "2.0");
        call.addMember("method", "Arithmetic.Sub");
        call.addMember("params", params);
        client_.Call(conn_, call, cb);
    }
    
    void Mul(double lhs, double rhs,  const ResponseCallback& cb)
    {
        json::Value params(json::TYPE_OBJECT);

        params.addMember("lhs", lhs);
    
        params.addMember("rhs", rhs);
    
        json::Value call(json::TYPE_OBJECT);
        call.addMember("jsonrpc", "2.0");
        call.addMember("method", "Arithmetic.Mul");
        call.addMember("params", params);
        client_.Call(conn_, call, cb);
    }
    
    void Div(double lhs, double rhs,  const ResponseCallback& cb)
    {
        json::Value params(json::TYPE_OBJECT);

        params.addMember("lhs", lhs);
    
        params.addMember("rhs", rhs);
    
        json::Value call(json::TYPE_OBJECT);
        call.addMember("jsonrpc", "2.0");
        call.addMember("method", "Arithmetic.Div");
        call.addMember("params", params);
        client_.Call(conn_, call, cb);
    }
    




private:
    RpcClient client_;
    ConnectionPtr conn_;
    ConnCallback cb_;
    MutexLock mutex_;
};

#endif
