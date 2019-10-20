/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-11 20:07
 * Last modified : 2019-10-20 20:24
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

template <json::ValueType type, json::ValueType... types>
bool checkType(json::ValueType t)
{
    if(t == type)
    {
        return true;
    }
    if(sizeof...(types) > 0)
    {
        return checkType<types...>(t);
    }
     return false;   
}

template <json::ValueType type>
bool checkType(json::ValueType t)
{
    return t == type;
}

template<json::ValueType... types>
bool findValue(const json::Value &request, const char* key, json::Value &value)
{
    json::Value::MemberIterator it = request.findMember(key);
    if(it == request.memberEnd())
    {
        return false;
    }
    if(checkType<types...>(it->value.getType()))
    {
        value = it->value;
        return true;
    }
    return false;
}

bool RpcServer::checkReqest(const ConnectionPtr& conn, const json::Value& request)
{
    json::Value id;
    if(!findValue<json::TYPE_INT32, json::TYPE_INT64, json::TYPE_NULL, json::TYPE_STRING>(request, "id", id))
    {
        handleError(conn, INVALID_REQUEST, 0);
        return false;
    }

    json::Value version;
    if(!findValue<json::TYPE_STRING>(request, "jsonrpc", version) || version.getString() != "2.0")
    {
        handleError(conn, INVALID_REQUEST, id.getInt32());
        return false;
    }

    json::Value method;
    if(!findValue<json::TYPE_STRING>(request, "method", method))
    {
        handleError(conn, METHOD_NOT_FOUND, id.getInt32());
        return false;
    }

    // jsonrpc, method, params, id
    if(request.getSize() != (3 + (request.findMember("params") != request.memberEnd())))
    {
        handleError(conn, INVALID_REQUEST, id.getInt32());
        return false;
    }
    return true;
}


void RpcServer::handleSingleRequest(const ConnectionPtr& conn, const json::Value& request)
{
    if(!checkReqest(conn, request))
        return;
    //"Arithmetic.Add"
    // 调用格式 service.method
    int id = request["id"].getInt32();
    std::string method(request["method"].getString());
    size_t pos = method.find('.');
    if(pos == method.npos)
    {
        handleError(conn, INVALID_REQUEST, id);
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
    method.erase(0, pos + 1);
    // TODO 调用对应service
    JSON_RPC_ERROR err = NONE;
    it->second->callProcedureRequest(method, request, std::bind(&RpcServer::onRpcResponse, this , conn, _1), err);
    if(err != NONE)
    {
        handleError(conn, err, id);
    }
}


bool RpcServer::checkNotify(const ConnectionPtr& conn, const json::Value& request)
{
    json::Value version;
    if(!findValue<json::TYPE_STRING>(request, "jsonrpc", version) || version.getString() != "2.0")
    {
        LOG_WARN << "RpcServer::handleSingleNotify() " << toIpPort(conn->peer()) << "Notify Error : " << "INVALID_REQUEST";
        return false;
    }

    json::Value method;
    if(!findValue<json::TYPE_STRING>(request, "method", method))
    {
        LOG_WARN << "RpcServer::handleSingleNotify() " << toIpPort(conn->peer()) << "Notify Error : " << "METHOD_NOT_FOUND";
        return false;
    }

    // jsonrpc, method, params
    if(request.getSize() != (2 + (request.findMember("params") != request.memberEnd())))
    {
        LOG_WARN << "RpcServer::handleSingleNotify() " << toIpPort(conn->peer()) << "Notify Error : " << "INVALID_REQUEST";
        return false;
    }
    return true;
}

// 通知错误并不会报错嗷
void RpcServer::handleSingleNotify(const ConnectionPtr& conn, const json::Value& request)
{
    if(!checkNotify(conn, request))
        return;
    //"Arithmetic.Add"
    // 调用格式 service.method
    
    std::string method(request["method"].getString());
    size_t pos = method.find('.');
    if(pos == method.npos)
    {
        LOG_WARN << "RpcServer::handleSingleNotify() " << toIpPort(conn->peer()) << "Notify Error : " << "INVALID_REQUEST";
        //handleError(conn, METHOD_NOT_FOUND, 0);
        return;
    }
    // 寻找service
    std::string service = method.substr(0, pos);
    ServiceMap::iterator it = serviceMap_.find(service);
    if(it == serviceMap_.end())
    {
        LOG_WARN << "RpcServer::handleSingleNotify() " << toIpPort(conn->peer()) << "Notify Error : " << "METHOD_NOT_FOUND";
        //handleError(conn, METHOD_NOT_FOUND, 0);
        return;
    }
    method.erase(0, pos + 1);
    // TODO 调用对应service
    JSON_RPC_ERROR err = NONE;
    it->second->callProcedureNotify(method, request, err);
    if(err != NONE)
    {
        LOG_WARN << "RpcServer::handleSingleNotify() " << toIpPort(conn->peer()) << "Notify Error : " << Singleton<RpcError>::Instance().errorMessage(err);
        //handleError(conn, err, 0);
    }
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
    LOG_WARN << "RpcServer::handleRequest() " << toIpPort(conn->peer()) << "Request Error : " << Singleton<RpcError>::Instance().errorMessage(err);
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
        LOG_TRACE << "onRpcResponse: ->" << toIpPort(conn->peer()) <<" notify success";
    }
}

