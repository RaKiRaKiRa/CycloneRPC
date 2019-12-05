/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-11-22 18:51
 * Last modified : 2019-11-22 20:49
 * Filename      : ClientStubGenerator.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONERPC_CLIENTSTUBGENERATOR_H
#define CYCLONERPC_CLIENTSTUBGENERATOR_H
#include "StubGenerator.h"

class ClientStubGenerator: public StubGenerator
{
public:
    explicit ClientStubGenerator(json::Value& proto):
        StubGenerator(proto)
    {}
    // 构造stub
    std::string genStub();
    // 构造的stub名
    std::string genStubClassName();
private:
    // 获得大写serviceName用于ifndef
    std::string genMacroName();

    // 在构造函数中定义被addProcedureRequest和addProcedureNotify调用的接口函数，其再调用用户接口
    std::string genStubProcedureRequestDefinitions();
    std::string genStubProcedureNotifyDefinitions();

    // 获得Bindings中，addProcedure时作为参数的 参数名(字符串)和参数类型
    template<typename RpcType>
    std::string genArgs(const RpcType& rpc, int appendComma);
    // 获得Definitions中调用用户接口时传入的参数（参数由paramsFromJsonArray()或paramsFromJsonObject()定义）
    template<typename RpcType>
    std::string genParamMembers(const RpcType& rpc);

};

#endif