/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-11 20:07
 * Last modified : 2019-10-11 23:52
 * Filename      : RpcServer.cc
 * Description   : 
 **********************************************************/

#include "RpcServer.h"
#include "jackson/jackson/Writer.h"
#include "jackson/jackson/Document.h"
#include "jackson/jackson/StringWriteStream.h"

void RpcServer::onMessage(const ConnectionPtr &conn, Buffer* buf)
{
    while(buf->readableBytes() >= Buffer::kHeaderLen)
    {
        const int32_t len = buf->readHeader();
        if(buf->readableBytes() >= len + Buffer::kHeaderLen)
        {
            buf->retrieve(Buffer::kHeaderLen);
            std::string message(buf->peek(), len);
            handleRequest(message);
            buf->retrieve(len);
        }
        else
        {
            break;
        }
    }
}

void RpcServer::handleRequest(const std::string& json)
{
    
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

