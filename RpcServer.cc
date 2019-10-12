/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-11 20:07
 * Last modified : 2019-10-12 16:45
 * Filename      : RpcServer.cc
 * Description   : 
 **********************************************************/

#include "RpcServer.h"
#include "jackson/jackson/Writer.h"
#include "jackson/jackson/Document.h"
#include "jackson/jackson/StringWriteStream.h"
#include "Cyclone/net/base/Logging.h"
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
        case json::TYPE_OBJECT:
            if(isNotify(request))
                handleSingleNotify(conn, request);
            else
                handleSingleRequest(conn, request);
            break;
        case json::TYPE_ARRAY:
            handleBatchRequests(conn, request);
            break;
        default:
            handleError(conn, INVALID_REQUEST, 0, "request should be json object or array");    
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
    // 响应报文构造完成，发送报文
    LOG_WARN << "RpcServer::handleRequest() " << toIpPort(conn->peer()) << "request : " << Singleton<RpcError>::Instance().errorMessage(err);
    sendResponse(conn, response);
    conn->shutdown();
    
}


void RpcServer::sendResponse(const ConnectionPtr &conn, json::Value &response)
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

