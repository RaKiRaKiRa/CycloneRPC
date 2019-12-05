/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-26 16:45
 * Last modified : 2019-11-22 19:27
 * Filename      : stubGenerator.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONEPRC_STUBGENERATOR_H
#define CYCLONEPRC_STUBGENERATOR_H
#include <Value.h>
//#include "../jackson/jackson/Value.h"
#include "StubException.h"

class StubGenerator
{
public:
    explicit StubGenerator(json::Value& proto)
    {
        parseProto(proto);
    }
    virtual ~StubGenerator() = default;

    // 构造stub
    virtual std::string genStub() = 0;
    virtual std::string genStubClassName() = 0;

protected:
    struct RpcRequest
    {
        RpcRequest(const std::string& nameP, json::Value& paramsP, json::Value& returnP):
            name(nameP),
            params(paramsP),
            ret(returnP)
        {}

        std::string name;
        json::Value params;
        json::Value ret;
    };
    struct RpcNotify
    {
        RpcNotify(const std::string& nameP, json::Value& paramsP):
            name(nameP),
            params(paramsP)
        {}

        std::string name;
        json::Value params;
    };

    struct Service
    {
        std::string name;
        std::vector<RpcRequest> rpcRequest;
        std::vector<RpcNotify> rpcNotify;
    };

    // 用于存放Service的name和Procedure
    Service service_;
private:
    void parseProto(json::Value& proto);
    void parseRpc(json::Value& Rpc);
    // 确认没有重复参数或空参数
    void checkParams(json::Value& params);
    // 确认返回值是非null 非array
    void checkReturn(json::Value& ret);
};


inline void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    size_t pos = 0;
    while((pos = str.find(from, pos)) != std::string::npos)
    {
        str.replace(pos, from.size(), to);
        pos += to.size();
    }
}

#endif