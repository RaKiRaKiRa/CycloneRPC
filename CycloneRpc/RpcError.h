/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-12 15:33
 * Last modified : 2019-11-23 20:27
 * Filename      : RpcError.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONE_RPCERROR_H
#define CYCLONE_RPCERROR_H
#include <Cyclone/net/base/Singleton.h>
#include <stdint.h>
#include <string>
#include <map>

enum JSON_RPC_ERROR
{
    NONE             = 0,
    PRASE_ERROR      = -32700,
    INVALID_REQUEST  = -32600,
    METHOD_NOT_FOUND = -32601,
    INVALID_PARAMS   = -32602,
    INTERNAL_ERROR   = -32603
};

class RpcError
{
public:
    RpcError()
    {
        ErrorString[PRASE_ERROR]      = "Parse error";
        ErrorString[INVALID_REQUEST]  = "Invalid request";
        ErrorString[METHOD_NOT_FOUND] = "Method not found";
        ErrorString[INVALID_PARAMS]   = "Invalid params";
        ErrorString[INTERNAL_ERROR]   = "Internal error";
    }

    int32_t errorCode(JSON_RPC_ERROR err) const
    {
        return err;
    }

    std::string errorMessage(JSON_RPC_ERROR err) const
    {
        std::map<JSON_RPC_ERROR ,std::string>::const_iterator it = ErrorString.find(err);
        if(it != ErrorString.end())
        {
            return it->second;
        }
        return "Unkowned error";
    }
private:
    std::map<JSON_RPC_ERROR ,std::string> ErrorString;
};

#endif