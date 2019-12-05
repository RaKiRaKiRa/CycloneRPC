/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-27 15:55
 * Last modified : 2019-11-06 02:44
 * Filename      : ServerStubGenerator.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONERPC_SERVERSTUBGENERATOR_H
#define CYCLONERPC_SERVERSTUBGENERATOR_H
#include "StubGenerator.h"

class ServerStubGenerator: public StubGenerator
{
public:
    explicit ServerStubGenerator(json::Value& proto):
        StubGenerator(proto)
    {}
    // 构造stub
    std::string genStub();
    // 构造的stub名
    std::string genStubClassName();
private:
    // 获得大写serviceName用于ifndef
    std::string genMacroName();
    // 获得子类name，用于获得子类指针调用用户定义接口，用户必须按规则设置classname，即[service_.name]Service
    std::string genUserClassName();
    // 在构造函数中service->addProcedureRequest和service->addProcedureNotify
    std::string genStubProcedureRequestBindings();
    std::string genStubProcedureNotifyBindings();
    // 在构造函数中定义被addProcedureRequest和addProcedureNotify调用的接口函数，其再调用用户接口
    std::string genStubProcedureRequestDefinitions();
    std::string genStubProcedureNotifyDefinitions();

    //TODO
    // 获得Definitions接口名
    template<typename RpcType>
    std::string genStubName(const RpcType& rpc);
    // 获得Bindings中，addProcedure时作为参数的 参数名(字符串)和参数类型
    template<typename RpcType>
    std::string genParamsAndType(const RpcType& rpc);
    // 获得Definitions中调用用户接口时传入的参数（参数由paramsFromJsonArray()或paramsFromJsonObject()定义）
    template<typename RpcType>
    std::string genArgs(const RpcType& rpc);

    template<typename RpcType>
    std::string genParamsFromJsonArray(const RpcType& rpc);
    template<typename RpcType>
    std::string genParamsFromJsonObject(const RpcType& rpc);
};

#endif