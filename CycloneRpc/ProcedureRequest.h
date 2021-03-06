/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-20 01:23
 * Last modified : 2019-12-06 03:24
 * Filename      : ProcedureRequest.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONERPC_PROCEDUREREQUEST_H
#define CYCLONERPC_PROCEDUREREQUEST_H

#include "common.h"
#include "RpcError.h"


class ProcedureRequest: noncopyable
{
public:
    template<typename... ParamNameAndParamType>
    explicit ProcedureRequest(ProcedureRequestCallback&& cb, ParamNameAndParamType&&... nameAndTypes):
        requestCallback_(cb)
    {
        const int n = sizeof...(nameAndTypes);
        // nameAndTypes必有偶数个，一个name，一个type
        static_assert(n % 2 == 0, "n % 2 != 0");
        // 递归展开存入params_
        if constexpr (n > 0)
            initProcedure(nameAndTypes...);
    }

    void invoke(const json::Value& request, JSON_RPC_ERROR& err, RpcDoneCallback done)
    {
        if(checkRequest(request, err))
            requestCallback_(request, done);
    }
    
private:
    bool checkRequest(const json::Value& request,  JSON_RPC_ERROR& err)
    {
        switch(request.getType())
        {
            case json::TYPE_OBJECT:
            case json::TYPE_ARRAY:
                if(!checkParam(request))
                {
                    err = INVALID_PARAMS;
                    return false;
                }
                break;
            default:
                err = INVALID_REQUEST;
                return false;
                break;
        }
        return true;
    }

    // 将nameAndTypes...的前两个参数作为普通参数，用于推导
    // 递归展开参数包，将前两项作为普通参数，最后一次由于没有参数包，会调用没有参数包的重载函数
    template<typename... ParamNameAndTypes>
    void initProcedure(const std::string& name, const json::ValueType& type, ParamNameAndTypes&&... nameAndTypes)
    {
        params_.emplace_back(name, type);
        initProcedure(nameAndTypes...);
    }

    void initProcedure(const std::string& name, const json::ValueType& type)
    {
        params_.emplace_back(name, type);
    }

    bool checkParam(const json::Value& request) const;

    struct Param
    {
        Param(const std::string &name, const json::ValueType &type):
            paramName(name),
            paramType(type)
        {}

        std::string paramName;
        json::ValueType paramType;
    };

    std::vector<Param> params_;
    ProcedureRequestCallback requestCallback_;
};

#endif