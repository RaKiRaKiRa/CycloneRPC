/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-28 15:51
 * Last modified : 2019-11-06 04:58
 * Filename      : ServerStubGenerator.cc
 * Description   : 
 **********************************************************/
#include "ServerStubGenerator.h"

#include <stdio.h>
std::string ServerStubGenerator::genStub()
{
    std::string macroName     = genMacroName();
    std::string userClassName = genUserClassName();
    std::string stubClassName = genStubClassName();
    std::string serviceName   = service_.name;

    std::string stubProcedureBindings = genStubProcedureRequestBindings();
    stubProcedureBindings.append(genStubProcedureNotifyBindings());

    std::string stubProcedureDefinitions = genStubProcedureRequestDefinitions();
    stubProcedureBindings.append(genStubProcedureNotifyDefinitions());

    // stub模板
    std::string stubTemplate = R"(
        #ifndef CYCLONERPC_[macroName]_H
        #define CYCLONERPC_[macroName]_H
        #include <jackson/Value>
        #include <CycloneRpc/RpcService.h>
        #include <CycloneRpc/RpcServer.h>

        class [userClassName];

        template <typename userClassName>
        class [stubClassName]: noncopyable
        {
        protected:
            explicit [stubClassName](RpcServer &server)
            {
                RpcService* service = new RpcService;

[stubProcedureBindings]

                server.addService("[serviceName]", service);
            }
        private:
[stubProcedureDefinitions]

            userClassName& service()
            {
                return static_cast<userClassName&>(*this);
            }
        };
        #endif
    )";
    // 替换stub模板
    replaceAll(stubTemplate, "[macroName]", macroName);
    replaceAll(stubTemplate, "[userClassName]", userClassName);
    replaceAll(stubTemplate, "[stubClassName]", stubClassName);
    replaceAll(stubTemplate, "[serviceName]", serviceName);
    replaceAll(stubTemplate, "[stubProcedureBindings]", stubProcedureBindings);
    replaceAll(stubTemplate, "[stubProcedureDefinitions]", stubProcedureDefinitions);
    return stubTemplate;
}

std::string ServerStubGenerator::genStubProcedureRequestBindings()
{
    std::string requestTemplate = R"(
                service->addProcedureRequest("[procedureName]", 
                        new ProcedureRequest(std::bind(&[stubClassName]::[stubProcedureName], this, _1, _2)
                        [procedureParams])
                        );
)";
    std::string res;
    for(size_t i = 0; i < service_.rpcRequest.size(); ++i)
    {
        std::string binding = requestTemplate;
        std::string procedureName = service_.rpcRequest[i].name;
        std::string stubClassName = genStubClassName();
        std::string stubProcedureName = genStubName(service_.rpcRequest[i]);
        std::string procedureParams = genParamsAndType(service_.rpcRequest[i]);
        replaceAll(binding, "[procedureName]", procedureName);
        replaceAll(binding, "[stubClassName]", stubClassName);
        replaceAll(binding, "[stubProcedureName]", stubProcedureName);
        replaceAll(binding, "[procedureParams]", procedureParams);
        res.append(binding);
        res.append("\n");
    }
    return res;
}

std::string ServerStubGenerator::genStubProcedureNotifyBindings()
{
    std::string notifyTemplate = R"(
                service->addProcedureNotify("[procedureName]", 
                        new ProcedureNotify(std::bind(&[stubClassName]::[stubProcedureName], this, _1)
                        [procedureParams])
                        );
)";
    std::string res;
    size_t sz = service_.rpcNotify.size();
    for(size_t i = 0; i < sz; ++i)
    {
        std::string binding = notifyTemplate;
        std::string procedureName = service_.rpcNotify[i].name;
        std::string stubClassName = genStubClassName();
        std::string stubProcedureName = genStubName(service_.rpcNotify[i]);
        std::string procedureParams = genParamsAndType(service_.rpcNotify[i]);
        replaceAll(binding, "[procedureName]", procedureName);
        replaceAll(binding, "[stubClassName]", stubClassName);
        replaceAll(binding, "[stubProcedureName]", stubProcedureName);
        replaceAll(binding, "[procedureParams]", procedureParams);
        res.append(binding);
        res.append("\n");
    }
    return res;
}

std::string ServerStubGenerator::genStubProcedureRequestDefinitions()
{
    std::string ArgsTemplate = R"(
            void [stubProcedureName](json::Value& request, const RpcDoneCallback& done)
            {
                json::Value params = request["params"];
                if(params.isArray())
                {
[paramsFromJsonArray];
                    service().[procedureName]([procedureArgs] UserDoneCallback(request, done));
                }
                else
                {
[paramsFromJsonObject];
                    service().[procedureName]([procedureArgs] UserDoneCallback(request, done));
                }
            }
)";
    std::string noArgTemplate = R"(
            void [stubProcedureName](json::Value& request, const RpcDoneCallback& done)
            {
                service().[procedureName](UserDoneCallback(request, done));
            }
)";

    std::string res;
    for(size_t i = 0; i < service_.rpcRequest.size(); ++i)
    {
        std::string procedureName = service_.rpcRequest[i].name;
        std::string stubProcedureName = genStubName(service_.rpcRequest[i]);
        if(service_.rpcRequest[i].params.getSize() > 0)
        {
            // 有参数
            std::string definitions = ArgsTemplate;
            std::string paramsFromJsonArray = genParamsFromJsonArray(service_.rpcRequest[i]);
            std::string paramsFromJsonObject = genParamsFromJsonObject(service_.rpcRequest[i]);
            std::string procedureArgs = genArgs(service_.rpcRequest[i]);
            replaceAll(definitions, "[procedureName]", procedureName);
            replaceAll(definitions, "[stubProcedureName]", stubProcedureName);
            replaceAll(definitions, "[paramsFromJsonArray]", paramsFromJsonArray);
            replaceAll(definitions, "[paramsFromJsonObject]", paramsFromJsonObject);
            replaceAll(definitions, "[procedureArgs]", procedureArgs);
            res.append(definitions);
            res.append("\n");
        }
        else
        {
            // 无参数
            std::string definitions = noArgTemplate;
            replaceAll(definitions, "[procedureName]", procedureName);
            replaceAll(definitions, "[stubProcedureName]", stubProcedureName);
            res.append(definitions);
            res.append("\n");
        }
    }
    return res;
}

std::string ServerStubGenerator::genStubProcedureNotifyDefinitions()
{
    std::string ArgsTemplate = R"(
            void [stubProcedureName](json::Value& request)
            {
                json::Value params = request["params"];
                if(params.isArray())
                {
[paramsFromJsonArray]
                    service().[procedureName]([procedureArgs]);
                }
                else
                {
[paramsFromJsonObject]
                    service().[procedureName]([procedureArgs]);
                }
            }
)";
    std::string noArgTemplate = R"(
            void [stubProcedureName](json::Value& request)
            {
                service().[procedureName]();
            }
)";

    std::string res;
    for(size_t i = 0; i < service_.rpcNotify.size(); ++i)
    {
        std::string procedureName = service_.rpcNotify[i].name;
        std::string stubProcedureName = genStubName(service_.rpcNotify[i]);
        if(service_.rpcNotify[i].params.getSize() > 0)
        {
            // 有参数
            std::string definitions = ArgsTemplate;
            std::string paramsFromJsonArray = genParamsFromJsonArray(service_.rpcNotify[i]);
            std::string paramsFromJsonObject = genParamsFromJsonObject(service_.rpcNotify[i]);
            std::string procedureArgs = genArgs(service_.rpcNotify[i]);
            replaceAll(definitions, "[procedureName]", procedureName);
            replaceAll(definitions, "[stubProcedureName]", stubProcedureName);
            replaceAll(definitions, "[paramsFromJsonArray]", paramsFromJsonArray);
            replaceAll(definitions, "[paramsFromJsonObject]", paramsFromJsonObject);
            replaceAll(definitions, "[procedureArgs]", procedureArgs);
            res.append(definitions);
            res.append("\n");
        }
        else
        {
            // 无参数
            std::string definitions = noArgTemplate;
            replaceAll(definitions, "[procedureName]", procedureName);
            replaceAll(definitions, "[stubProcedureName]", stubProcedureName);
            res.append(definitions);
            res.append("\n");
        }
    }
    return res;
}

std::string ServerStubGenerator::genStubClassName()
{
    return service_.name + "Service";
}

std::string ServerStubGenerator::genUserClassName()
{
    return service_.name + "ServiceStub";
}

std::string ServerStubGenerator::genMacroName()
{
    std::string res = service_.name;
    for(size_t i = 0; i < res.size(); ++i)
    {
        res[i] = static_cast<char>(toupper(res[i]));
    }
    return res.append("SERVICESTUB");
}

template<typename RpcType>
std::string ServerStubGenerator::genStubName(const RpcType& rpc)
{
    return rpc.name + "Stub";
}

template<typename RpcType>
std::string ServerStubGenerator::genParamsAndType(const RpcType& rpc)
{
    std::string res;
    for(size_t i = 0; i < rpc.params.getObject().size(); ++i)
    {
        /*
        service->addProcedureNotify("[procedureName]", 
                        new ProcedureNotify(std::bind(&[stubClassName]::[stubProcedureName], this, _1)
                    ->  [procedureParams])
                        );
        */
        json::Member param = rpc.params.getObject()[i];
        std::string arg = "\"" + param.key.getString() + "\"";
        std::string type = "bad type";
        switch(param.value.getType())
        {
            case json::TYPE_BOOL:
                type = "json::TYPE_BOOL";
                break;
            case json::TYPE_DOUBLE:
                type = "json::TYPE_DOUBLE";
                break;
            case json::TYPE_INT64:
                type = "json::TYPE_INT64";
                break;
            case json::TYPE_INT32:
                type = "json::TYPE_INT32";
                break;
            case json::TYPE_STRING:
                type = "json::TYPE_STRING";
                break;
            case json::TYPE_OBJECT:
                type = "json::TYPE_OBJECT";
                break;
            case json::TYPE_ARRAY:
                type = "json::TYPE_ARRAY";
                break;
        };
        res.append(", ").append(arg);
        res.append(", ").append(type);
    }
    return res;
}

template<typename RpcType>
std::string ServerStubGenerator::genArgs(const RpcType& rpc)
{
    /*
            void [stubProcedureName](json::Value& request)
            {
                json::Value params = request["params"];
                if(params.isArray())
                {
[paramsFromJsonArray]
                    service().[procedureName](->[procedureArgs]<-);
                }
                else
                {
[paramsFromJsonObject]
                    service().[procedureName](->[procedureArgs]<-);
                }
            }
    */
    std::string res;
    for(size_t i = 0; i < rpc.params.getObject().size(); ++i)
    {
        std::string arg = rpc.params.getObject()[i].key.getString();
        res.append(arg).append(", ");
    }
    return res;
}

/*
            void [stubProcedureName](json::Value& request)
            {
                json::Value params = request["params"];
                if(params.isArray())
                {
->[paramsFromJsonArray]<-;
                    service().[procedureName]([procedureArgs]);
                }
                else
                {
->[paramsFromJsonObject]<-;
                    service().[procedureName]([procedureArgs]);
                }
            }
    */



template<typename RpcType>
std::string ServerStubGenerator::genParamsFromJsonArray(const RpcType& rpc)
{
    std::string res;
    for(size_t i = 0; i < rpc.params.getObject().size(); ++i)
    {
        std::string arg = rpc.params.getObject()[i].key.getString();
        std::string type = "bad type";
        switch(rpc.params.getObject()[i].value.getType())
        {
            case json::TYPE_BOOL:
                type = "Bool";
                break;
            case json::TYPE_DOUBLE:
                type = "Double";
                break;
            case json::TYPE_INT64:
                type = "Int64";
                break;
            case json::TYPE_INT32:
                type = "Int32";
                break;
            case json::TYPE_STRING:
                type = "String";
                break;
            /* TODO:
            case json::TYPE_OBJECT:
                type = "Object";
                break;
            case json::TYPE_ARRAY:
                type = "Array";
                break;
            */
        }
        char buf[120];
        snprintf(buf, sizeof buf, "auto %s = params[%d].get%s();\n", arg.c_str(), (int)i, type.c_str());
        res.append(buf);
    }
    return res;
}


template<typename RpcType>
std::string ServerStubGenerator::genParamsFromJsonObject(const RpcType& rpc)
{
    std::string res;
    for(size_t i = 0; i < rpc.params.getObject().size(); ++i)
    {
        std::string arg = rpc.params.getObject()[i].key.getString();
        std::string type = "bad type";
        switch(rpc.params.getObject()[i].value.getType())
        {
            case json::TYPE_BOOL:
                type = "Bool";
                break;
            case json::TYPE_DOUBLE:
                type = "Double";
                break;
            case json::TYPE_INT64:
                type = "Int64";
                break;
            case json::TYPE_INT32:
                type = "Int32";
                break;
            case json::TYPE_STRING:
                type = "String";
                break;
            /* TODO:
            case json::TYPE_OBJECT:
                type = "Object";
                break;
            case json::TYPE_ARRAY:
                type = "Array";
                break;
            */
        }
        char buf[120];
        snprintf(buf, sizeof buf, "auto %s = params[\"%s\"].get%s();\n", arg.c_str(), arg.c_str(), type.c_str());
        res.append(buf);
    }
    return res;
}