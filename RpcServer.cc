/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-11 20:07
 * Last modified : 2019-10-18 16:12
 * Filename      : RpcServer.cc
 * Description   : 
 **********************************************************/

#include "RpcServer.h"
#include "jackson/jackson/Writer.h"
#include "jackson/jackson/Document.h"
#include "jackson/jackson/StringWriteStream.h"
#include "Cyclone/net/base/Logging.h"
#include "Cyclone/net/Server.h"
#include "RpcService.h"
#include "common.h"
#include <memory>


RpcServer::RpcServer(EventLoop* loop, const sockaddr_in&  listen, std::string name, bool ReusePort):
    server_(loop, listen, name, ReusePort),
    serviceMap_()
{
    server_.setMessCallback(std::bind(&RpcServer::onMessage, this, _1, _2));
    server_.setConnCallback(std::bind(&RpcServer::onConnection, this, _1));
    LOG_INFO << "RpcServer::RpcServer " << name;
}




void RpcServer::addService(const std::string& serviceName, RpcService* service)
{
    LOG_INFO << "RpcServer::addService : " << serviceName; 
    serviceMap_.emplace(serviceName, service);
}

void RpcServer::onConnection(const ConnectionPtr &conn)
{
    if(conn->connected())
    {
        LOG_TRACE << "connection " << toIpPort(conn->peer()) << " is UP";
    }
    else
    {
        LOG_TRACE << "connection " << toIpPort(conn->peer()) << " is DOWN";
    }
    
}

void RpcServer::onMessage(const ConnectionPtr &conn, Buffer* buf)
{
    while(buf->readableBytes() >= Buffer::kHeaderLen)
    {
        const int32_t len = buf->readHeader();
        if(buf->readableBytes() >= len + Buffer::kHeaderLen)
        {
            buf->retrieve(Buffer::kHeaderLen);
            std::string message(buf->peek(), len);
            handleRequest(conn, message);
            buf->retrieve(len);
        }
        else
        {
            break;
        }
    }
}

void RpcServer::handleRequest(const ConnectionPtr &conn, const std::string& json)
{
    json::Document request;
    // 解析string为json
    json::ParseError err = request.parse(json);
    // request格式错误，PRASE_ERROR
    if(err != json::PARSE_OK)
    {
        handleError(conn, PRASE_ERROR, 0, json::parseErrorStr(err));
        
        return;
    }
    /*
    //批量调用
    [
    {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
    {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},
    {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
    {"foo": "boo"},
    {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
    {"jsonrpc": "2.0", "method": "get_data", "id": "9"} 
    ]

    //Notify调用(没有id)
    --> {"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]}
 
    //Return调用(有id)
    --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
    <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
    
    -> {"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3}
    <-- {"jsonrpc": "2.0", "result": 19, "id": 3}
    */
    switch(request.getType())
    {
        // 非批量调用
        case json::TYPE_OBJECT:
            if(isNotify(request))
                handleSingleNotify(conn, request);
            else
                handleSingleRequest(conn, request);
            break;
        // 批量调用
        case json::TYPE_ARRAY:
            handleBatchRequests(conn, request);
            break;
        default:
            handleError(conn, INVALID_REQUEST, 0, "request should be json object or array");    
    }
}

void RpcServer::handleBatchRequests(const ConnectionPtr& conn, const json::Value& requests)
{
    size_t n = requests.getSize();
    if(0 == n)
    {
        handleError(conn, INVALID_REQUEST, 0, "batch request is empty");
        return;
    }
    // 对每个请求调用
    for(int i = 0; i < n; ++i)
    {
        json::Value request = requests[i];
        if(!request.isObject())
        {
            handleError(conn, INVALID_REQUEST, 0, "request should be json object");
            continue;
        }
        if(isNotify(request))
            handleSingleNotify(conn, request);
        else
            handleSingleRequest(conn, request);
    }
}

void RpcServer::handleSingleRequest(const ConnectionPtr& conn, const json::Value& request)
{
    if(!checkReqest(request))
        return;
    //"Arithmetic.Add"
    // 调用格式 service.method
    int id = request["id"].getInt32();
    std::string method(request["method"].getString());
    size_t pos = method.find('.');
    if(pos == method.npos)
    {
        handleError(conn, METHOD_NOT_FOUND, id);
        return;
    }
    // 寻找service
    std::string service = method.substr(0, pos);
    ServiceMap::iterator it = serviceMap_.find(service);
    if(it == serviceMap_.end())
    {
        handleError(conn, METHOD_NOT_FOUND, id);
        return;
    }
    method.erase(method.begin() + pos, method.end());
    // TODO 调用对应service
    it->second->callProcedureRequest(method, request, std::bind(&RpcServer::onRpcResponse, this , conn, _1));
}

void RpcServer::handleSingleNotify(const ConnectionPtr& conn, const json::Value& request)
{
    if(!checkNotify(request))
        return;
    //"Arithmetic.Add"
    // 调用格式 service.method
    int id = request["id"].getInt32();
    std::string method(request["method"].getString());
    size_t pos = method.find('.');
    if(pos == method.npos)
    {
        handleError(conn, METHOD_NOT_FOUND, id);
        return;
    }
    // 寻找service
    std::string service = method.substr(0, pos);
    ServiceMap::iterator it = serviceMap_.find(service);
    if(it == serviceMap_.end())
    {
        handleError(conn, METHOD_NOT_FOUND, id);
        return;
    }
    method.erase(method.begin() + pos, method.end());
    // TODO 调用对应service
    it->second->callProcedureNotify(method, request);
}


void RpcServer::handleError(const ConnectionPtr& conn, JSON_RPC_ERROR err, int32_t id, std::string data)
{
    //{"jsonrpc": "2.0", "error": {"code": -32700, "message": "Parse error"}, "id": null}
    json::Value response(json::TYPE_OBJECT);
    json::Value error(json::TYPE_OBJECT);
    response.addMember("jsonrpc", "2.0");
    error.addMember("code", Singleton<RpcError>::Instance().errorCode(err));
    error.addMember("message", Singleton<RpcError>::Instance().errorMessage(err));
    if(!data.empty())
        error.addMember("data", data);
    response.addMember("error", error);
    response.addMember("id", id);
    // 响应报文构造完成，发送错误报文
    LOG_WARN << "RpcServer::handleRequest() " << toIpPort(conn->peer()) << "request : " << Singleton<RpcError>::Instance().errorMessage(err);
    sendResponse(conn, response);
    conn->shutdown();
    
}


void RpcServer::sendResponse(const ConnectionPtr &conn, const json::Value &response)
{
    json::StringWriteStream os;
    json::Writer writer(os);
    response.writeTo(writer);
    
    Buffer message;
    message.append(os.get());
    // 加入包头防止粘包
    message.setHeader(os.get().length());
    conn->send(&message);
}


void RpcServer::onRpcResponse(const ConnectionPtr& conn, const json::Value& response)
{
    if(!response.isNull())
    {
        sendResponse(conn, response);
        LOG_TRACE << "onRpcResponse: ->" << toIpPort(conn->peer()) <<" request success";
    }
    else
    {
        LOG_TRACE << "onRpcResponse: ->" << toIpPort(conn->peer()) <<" ontify success";
    }
}

