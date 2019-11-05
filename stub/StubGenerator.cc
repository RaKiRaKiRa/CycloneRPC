/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-26 17:31
 * Last modified : 2019-11-06 04:56
 * Filename      : StubGenerator.cc
 * Description   : 
 **********************************************************/
#include "StubGenerator.h"
#include <unordered_set>

void StubGenerator::parseProto(json::Value& proto)
{
    //整个proto是一个object{}， rpc是个包括多个object{}的array[]，其中params也是一个object{}
    stubExpect(proto.isObject(), "proto must be json-object");
    stubExpect(proto.getSize() == 2, "proto object must just contain name and rpc");

    json::Value::MemberIterator name = proto.findMember("name");
    stubExpect(name != proto.memberEnd(), "missing service name");
    stubExpect(name->value.isString(), "name must be string");
    service_.name = name->value.getString();

    json::Value::MemberIterator rpc = proto.findMember("rpc");
    stubExpect(rpc != proto.memberEnd(), "missing service rpc");
    stubExpect(rpc->value.isArray(), "rpc must be array");

    size_t numOfProcedure = rpc->value.getSize();
    for(size_t i = 0; i < numOfProcedure; ++i)
    {
        parseRpc(rpc->value[i]);
    }
}
void StubGenerator::parseRpc(json::Value& rpc)
{
    // 一个rpc必须有name，但可以没有params或return
    stubExpect(rpc.isObject(), "rpc must be json-object");
    json::Value::MemberIterator name = rpc.findMember("name");
    stubExpect(name != rpc.memberEnd(), "missing rpc name");
    stubExpect(name->value.isString(), "rpc name must be string");

    json::Value::MemberIterator params = rpc.findMember("params");
    json::Value paramsValue = json::Value(json::TYPE_OBJECT);
    // 若有params， 则检查
    if(params != rpc.memberEnd())
    {
        checkParams(params->value);
        paramsValue = params->value;
    }

    // 检查return并将RpcProcedure加入service_
    json::Value::MemberIterator ret = rpc.findMember("return");
    if(ret != rpc.memberEnd())
    {
        //std::cout<<"RpcRequest"<<std::endl;
        checkReturn(ret->value);
        RpcRequest request(name->value.getString(), paramsValue, ret->value);
        service_.rpcRequest.push_back(request);
    }
    else
    {
        //std::cout<<"RpcNotify"<<std::endl;
        RpcNotify notify(name->value.getString(), paramsValue);
        service_.rpcNotify.push_back(notify);
    }
    
}
void StubGenerator::checkParams(json::Value& params)
{
    const std::vector<json::Member> paramsList = params.getObject();
    std::unordered_set<std::string> set;
    for(size_t i = 0 ; i < paramsList.size(); ++i)
    {
        stubExpect(set.find(paramsList[i].key.getString()) == set.end(), "duplicate param name");
        stubExpect(paramsList[i].value.getType() != json::TYPE_NULL, "bad param type");
        set.insert(paramsList[i].key.getString());
    }
}


void StubGenerator::checkReturn(json::Value& ret)
{
    switch (ret.getType())
    {
    case json::TYPE_NULL:
    case json::TYPE_ARRAY:
        stubExpect(false, "bad return type");
        break;
    // "return":{}
    case json::TYPE_OBJECT:
        checkParams(ret);
        break;
    // "return":6.0
    // "return":"string"
    default:
        break;
    }
}
