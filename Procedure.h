/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-18 19:40
 * Last modified : 2019-10-18 21:07
 * Filename      : Procedure.h
 * Description   : 
 **********************************************************/

#ifndef CYCLONE_PROCEDURE_H
#define CYCLONE_PROCEDURE_H

#include "Cyclone/net/base/noncopyable.h"
#include "common.h"



//在[stubProcedureBindings]部分中，services->addProcedureReturn时会new Procedure(bind接口，[procedureParams])
//新构造的Procedure根据[procedureParams]将所需参数的参数名和参数类型存入Param，再将Param存入std::vector<Param> params_
//将bind接口作为回调
template<typename ProcedureType>
class Procedure : noncopyable
{
public:
    // C++11 参数包，不定参数数量
    template<typename... ParamNameAndParamType>
    explicit Procedure(ProcedureType cb, ParamNameAndParamType... nameAndTypes)：
        procedureCallback_(std::move(cb))
    {
        int n = sizeof...(nameAndTypes);
        // nameAndTypes必有偶数个，一个name，一个type
        static_assert(n % 2 == 0);
        // 递归展开存入params_
        if(n > 0)
            initProcedure(nameAndTypes...);
    }

    // 调用
    void invoke(json::Value& request, const RpcDoneCallback& done);
    void invoke(json::Value& request);
private:
    // 将nameAndTypes...的前两个参数作为普通参数，用于推导
    // 递归展开参数包，将前两项作为普通参数，最后一次由于没有参数包，会调用没有参数包的重载函数
    template<typename... ParamNameAndTypes>
    void initProcedure(const std::string& name, const json::ValueType& type, ParamNameAndTypes... nameAndTypes);

    void initProcedure(const std::string& name, const json::ValueType& type);

    // 当传入参数类型不是json::ValueType格式时 assert错误
    //template<typename Name, typename Type， typename... ParamNameAndTypes>
    //void initProcedure(Name paramName, Type type, ParamNameAndTypes... nameAndTypes)
    //template<typename Name>
    //void initProcedure(Name paramName, Type type);

    struct Param
    {
        Param(std::string &name, json::ValueType &type):
            paramName(name),
            paramType(type)
        {}

        std::string paramName;
        json::ValueType paramType;
    };

    ProcedureType procedureCallback_;
    std::vector<Param> params_;
};

#endif