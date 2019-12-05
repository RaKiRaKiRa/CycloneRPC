/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-11-20 15:36
 * Last modified : 2019-12-06 03:48
 * Filename      : RpcClient.cc
 * Description   : 
 **********************************************************/
#include "RpcClient.h"
#include <jackson/jackson/StringWriteStream.h>
#include <jackson/jackson/Writer.h>
#include <jackson/jackson/Document.h>

#include <Cyclone/net/base/Logging.h>
#include <limits.h>

void RpcClient::Call(const ConnectionPtr& conn, json::Value& call, const ResponseCallback& cb)
{
    call.addMember("id", id_);
    callbackMap_[id_] = cb;
    
    if(id_ == INT_MAX)
    {
        id_ = 1;
    }
    else
    {
        ++id_;
    }
    
    sendCall(conn, call);
}

void RpcClient::Call(const ConnectionPtr& conn, json::Value& call)
{
    //Notify不需要id
    sendCall(conn, call);
}

void RpcClient::sendCall(const ConnectionPtr& conn, const json::Value& request)
{
    //将json转string再发送
    json::StringWriteStream os;
    json::Writer writer(os);
    request.writeTo(writer);
    std::string message(os.get());
    Buffer buf;
    buf.append(message);
    // 设置包头防止粘包
    buf.setHeader(message.size());
    conn->send(&buf);
}

void RpcClient::onMessage(const ConnectionPtr& conn, Buffer* buf)
{
    // 解析时先看包头
    while(buf->readableBytes() >= Buffer::kHeaderLen)
    {
        const int32_t len = buf->getHeader();
        /*
        if(len <= 0)
        {
            LOG_ERROR << "Invalid Length "<<len;
            // 连接断开 全部丢弃
            conn->shutdown();
            break;
        }
        */
        if(buf->readableBytes() >= len + Buffer::kHeaderLen)
        {
            buf->retrieve(Buffer::kHeaderLen);
            std::string message(buf->peek(), len);
            ParseResponse(message);
            buf->retrieve(len);
        }
        else
        {
            break;
        }
    }
}

void RpcClient::ParseResponse(std::string& json)
{
    json::Document response;
    json::ParseError err = response.parse(json);
    if(err != json::PARSE_OK)
    {
        LOG_ERROR << "ParseResponse ERROR: " << json::parseErrorStr(err);
        return;
    }

    switch(response.getType())
    {
        // 只有一个响应
        case json::TYPE_OBJECT:
        {
            ParseSingleResponse(response);
            break;
        }
        // 批量响应
        case json::TYPE_ARRAY:
        {
            size_t n = response.getSize();
            if(n == 0)
            {
                LOG_ERROR << "Response ERROR: batch response is empty";
                return;
            }
            for(size_t i = 0; i < n; ++i)
            {
                ParseSingleResponse(response[i]);
            }
        }
        default:
        {
            LOG_ERROR << "Response ERROR: response should be json object or array";
            return;
        }
    }
}

void RpcClient::ParseSingleResponse(json::Value& response)
{
    if(checkResponse(response) == false)
    {
        return;
    }
    // 检查id
    int32_t id = response["id"].getInt32();
    CallbackMap::iterator it = callbackMap_.find(id);
    if(it == callbackMap_.end())
    {
        LOG_ERROR << "response "<< id <<" not found";
        return;
    }
    // 根据result或error回调
    json::Value::MemberIterator res = response.findMember("result");
    if(res != response.memberEnd())
    {
        it->second(res->value, false);
    }
    else
    {
        res = response.findMember("error");
        it->second(res->value, true);
    }
    callbackMap_.erase(it);
}

bool RpcClient::checkResponse(json::Value& response)
{
    if(response.getSize() != 3)
    {
        LOG_ERROR << "response should have exactly 3 field";
        // 删还是不删？？？
        // 删吧，服务器应该时只回复一次的
        json::Value::MemberIterator it = response.findMember("id");
        if(it != response.memberEnd() && it->value.getType() == json::TYPE_INT32)
        {
            if(callbackMap_.find(it->value.getInt32()) != callbackMap_.end())
                callbackMap_.erase(it->value.getInt32());
        }
        return false;
    }

    // id是否正确
    int32_t id;
    json::Value::MemberIterator it = response.findMember("id");
    if(it != response.memberEnd() && it->value.getType() == json::TYPE_INT32)
    {
        id = it->value.getInt32();
    }
    else
    {
        LOG_ERROR << "response should have id";
        return false;
    }
    
    // version是否正确
    std::string version;
    it = response.findMember("jsonrpc");
    if(it != response.memberEnd() && it->value.getType() == json::TYPE_STRING)
    {
        version = it->value.getString();
    }
    else
    {
        LOG_ERROR << "response should have version";
        if(callbackMap_.find(id) != callbackMap_.end())
            callbackMap_.erase(id);
        return false;
    }
    if(version != "2.0")
    {
        LOG_ERROR << "response should be version 2.0 than " << version;
        if(callbackMap_.find(id) != callbackMap_.end())
            callbackMap_.erase(id);
        return false;
    }

    //是否有result或error
    if (response.findMember("result") != response.memberEnd())
        return true;

    it = response.findMember("error");
    if(it != response.memberEnd() && it->value.getType() == json::TYPE_OBJECT)
    {
        return true;
    }
    // 没有result或error，错误
    LOG_ERROR << "response should have error or result ";
    if(callbackMap_.find(id) != callbackMap_.end())
        callbackMap_.erase(id);
    return false;
    
}

