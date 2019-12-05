/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-11-22 18:54
 * Last modified : 2019-11-23 21:03
 * Filename      : ClientStubGenerator.cc
 * Description   : 
 **********************************************************/
#include "ClientStubGenerator.h"

std::string ClientStubGenerator::genStub()
{
    std::string stubTemplate = R"(
#ifndef CYCLONERPC_[macroName]_H
#define CYCLONERPC_[macroName]_H
#include <CycloneRpc/RpcClient.h>
#include <jackson/jackson/Value.h>
#include <Cyclone/net/base/Logging.h>
class [stubClassName]: noncopyable
{
public:
    [stubClassName](EventLoop* loop, sockaddr_in& addr, const std::string& name = "[stubClassName]"):
        client_(loop, addr, name)
    {
        client_.setConnCallback(std::bind(&[stubClassName]::onConnection, this, _1));
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

[requestDefine]


[notifyDefine]

private:
    RpcClient client_;
    ConnectionPtr conn_;
    ConnCallback cb_;
    MutexLock mutex_;
};

#endif
)";
    replaceAll(stubTemplate, "[macroName]", genMacroName());
    replaceAll(stubTemplate, "[stubClassName]", genStubClassName());
    replaceAll(stubTemplate, "[requestDefine]", genStubProcedureRequestDefinitions());
    replaceAll(stubTemplate, "[notifyDefine]", genStubProcedureNotifyDefinitions());
    return stubTemplate;
}


std::string ClientStubGenerator::genStubClassName()
{
    return service_.name + "ClientStub";
}

std::string ClientStubGenerator::genMacroName()
{
    std::string res = service_.name;
    for(size_t i = 0; i < res.size(); ++i)
    {
        res[i] = static_cast<char>(toupper(res[i]));
    }
    return res.append("CLIENTSTUB");
}

std::string ClientStubGenerator::genStubProcedureRequestDefinitions()
{
    std::string requestTemplate = R"(
    void [procedureName]([requestArgs] const ResponseCallback& cb)
    {
        json::Value params(json::TYPE_OBJECT);
[paramMembers]
        json::Value call(json::TYPE_OBJECT);
        call.addMember("jsonrpc", "2.0");
        call.addMember("method", "[serviceName].[procedureName]");
        call.addMember("params", params);
        client_.Call(conn_, call, cb);
    }
    )";
    std::string serviceName = service_.name;
    replaceAll(requestTemplate, "[serviceName]", serviceName);
    std::string res;
    for(auto& rpc : service_.rpcRequest)
    {
        std::string str = requestTemplate;
        replaceAll(str, "[procedureName]", rpc.name);
        replaceAll(str, "[paramMembers]", genParamMembers(rpc));
        replaceAll(str, "[requestArgs]", genArgs(rpc, true));
        res.append(str);
    }
    return res;
}

std::string ClientStubGenerator::genStubProcedureNotifyDefinitions()
{
    std::string requestTemplate = R"(
    void [procedureName]([notifyArgs])
    {
        json::Value params(json::TYPE_OBJECT);
[paramMembers]
        json::Value call(json::TYPE_OBJECT);
        call.addMember("jsonrpc", "2.0");
        call.addMember("method", "[serviceName].[procedureName]");
        call.addMember("params", params);
        client_.Call(conn_, call);
    }
    )";
    std::string serviceName = service_.name;
    replaceAll(requestTemplate, "[serviceName]", serviceName);
    std::string res;
    for(auto& rpc : service_.rpcNotify)
    {
        std::string str = requestTemplate;
        replaceAll(str, "[procedureName]", rpc.name);
        replaceAll(str, "[paramMembers]", genParamMembers(rpc));
        replaceAll(str, "[notifyArgs]", genArgs(rpc, false));
        res.append(str);
    }
    return res;
}

std::string genParamMember(const std::string& paramName)
{
    std::string paramTemplate = R"(
        params.addMember("[paramName]", [paramName]);
    )";
    replaceAll(paramTemplate, "[paramName]", paramName);
    return paramTemplate;
}

template<typename RpcType>
std::string ClientStubGenerator::genParamMembers(const RpcType& rpc)
{
    std::string paramMembers;
    for(int i = 0; i < rpc.params.getObject().size(); ++i)
    {
        paramMembers.append(genParamMember(rpc.params.getObject()[i].key.getString()));
    }
    return paramMembers;
}



template<typename RpcType>
std::string ClientStubGenerator::genArgs(const RpcType& rpc, int appendComma)
{
    std::string res;
    bool first = true;
    for(int i = 0; i < rpc.params.getObject().size(); ++i)
    {
        std::string type = "bad type ";
        std::string arg = rpc.params.getObject()[i].key.getString();
        switch(rpc.params.getObject()[i].value.getType())
        {
            case json::TYPE_INT32:
                type = "int32_t ";
                break;
            case json::TYPE_INT64:
                type = "int64_t ";
                break;
            case json::TYPE_STRING:
                type = "std::string ";
                break;
            case json::TYPE_DOUBLE:
                type = "double ";
                break;
            case json::TYPE_BOOL:
                type = "bool ";
                break;
            case json::TYPE_OBJECT:
            case json::TYPE_ARRAY:
                type = "json::Value ";
                break;   
        }
        std::string one = type + arg;
        if(first)
        {
            first = false;
        }
        else
        {
            res.append(", ");
        }
        res.append(one);
    }
    if(appendComma && !first)
    {
        res.append(", ");
    }
    return res;
}