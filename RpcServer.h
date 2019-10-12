/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-11 20:47
 * Last modified : 2019-10-12 16:39
 * Filename      : RpcServer.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONE_RPCSERVER_H
#define CYCLONE_RPCSERVER_H
#include "Cyclone/net/Server.h"
#include "jackson/jackson/Value.h"
#include "RpcError.h"

typedef std::function<void(json::Value)> RpcDoneCallback;

class RpcService;

class RpcServer:noncopyable
{
public:
    RpcServer(EventLoop* loop, const sockaddr_in&  listen, std::string name = "RpcServer", bool ReusePort = true);

    ~RpcServer();

    // 由生成的stub使用
    void addService(std::string& json, const RpcService* service);


private:
    void onConnection(const ConnectionPtr& conn);

    // 接收信息先检查包头，确认接收完毕后,取出信息，调用handldRequest
    void onMessage(const ConnectionPtr& conn, Buffer* buf);

    // 转为json，再判断时批量调用还是ReturnProduce还是NotifyProduce
    void handleRequest(const ConnectionPtr &conn, const std::string& json);

    // 批量调用
    void handleBatchRequests(const ConnectionPtr &conn, const json::Value& request);

    

    void handleSingleRequest(const ConnectionPtr &conn, const json::Value& request);

    void handleSingleNotify(const ConnectionPtr &conn, const json::Value& request);

    void onWriteComplete(const ConnectionPtr& conn);

    void sendResponse(const ConnectionPtr& conn, json::Value& response);

    // 错误处理
    void handleError(const ConnectionPtr& conn, JSON_RPC_ERROR err, int32_t id , std::string data = "");

    bool isNotify(const json::Value& request)
    {
        return request.findMember("id") == request.memberEnd();
    }

    Server server_;

    //RpcDoneCallback rpcDoneCallback_;

};


#endif //CYCLONE_RPCSERVER_H
