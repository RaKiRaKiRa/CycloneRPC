/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-11-16 01:33
 * Last modified : 2019-11-18 02:21
 * Filename      : RpcClient.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONE_RPCCLIENT_H
#define CYCLONE_RPCCLIENT_H
#include "../Cyclone/net/Client.h"
#include "../jackson/jackson/Value.h"
typedef std::function<void(json::Value&, bool isError)> ResponseCallback;
// RPC客户端是否应该超时重发？
// tcp保证服务器一定能收到
// 但是如果json报文发送时错误的怎么办？

class RpcClient: noncopyable
{
    typedef std::unordered_map<int64_t, ResponseCallback> CallbackMap;
public:
    RpcClient(EventLoop* loop, sockaddr_in& serverAddr, const std::string& name = "RpcClient"):
        client_(loop, serverAddr),
        id_(0)
    {
        client_.setMessCallback(std::bind(&RpcClient::onMessage, this, _1, _2));
    }
    RpcClient(EventLoop* loop, std::string ip, uint16_t port, const std::string& name = "RpcClient"):
        client_(loop, ip, port),
        id_(0)
    {
        client_.setMessCallback(std::bind(&RpcClient::onMessage, this, _1, _2));
    }
    RpcClient(EventLoop* loop, uint16_t port, const std::string& name = "RpcClient"):
        client_(loop, port),
        id_(0)
    {
        client_.setMessCallback(std::bind(&RpcClient::onMessage, this, _1, _2));
    }

    void setConnCallback(ConnCallback cb)
    {
        client_.setConnCallback(cb);
    }

    void start()
    {
        client_.connect();
    }

    // 将相应请求转为json然后调用sendCall
    // 收到返回值后调用cb->异步rpc =>需要一个表
    void Call(const ConnectionPtr& conn, json::Value& call, const ResponseCallback& cb);
    
    void Call(const ConnectionPtr& conn, json::Value& call);
private:
    // 解析回复一套
    void onMessage(const ConnectionPtr& conn, Buffer* buffer);
    void handleMessage(Buffer* buf);
    void handleResponse(std::string& json);
    void handleSingleResponse(json::Value& response);
    
    // 发送json
    void sendCall(const ConnectionPtr& conn, json::Value& request);

    CallbackMap callbackMap_;
    Client client_;
    int64_t id_;
    std::string name_;
};



#endif  CYCLONE_RPCCLIENT_H