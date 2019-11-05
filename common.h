/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-14 18:36
 * Last modified : 2019-10-28 20:34
 * Filename      : common.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONE_RPCCOMMON_H
#define CYCLONE_RPCCOMMON_H
#include <functional>
#include <memory>
#include "jackson/jackson/Value.h"
#include "Cyclone/net/Connection.h"
#include "Cyclone/net/Server.h"

class RpcService;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

typedef std::unique_ptr<RpcService> RpcServeicPtr;

typedef std::function<void(const json::Value&)> RpcDoneCallback;

typedef std::function<void(const json::Value&, const RpcDoneCallback&)> ProcedureRequestCallback;
typedef std::function<void(const json::Value&)> ProcedureNotifyCallback;

// 用于调用RpcDoneCallback时自动填充jsonrpc2.0 id 和 result
class UserDoneCallback
{
public:
    UserDoneCallback(json::Value &request, const RpcDoneCallback &cb):
        request_(request),
        callback_(cb)
    {}

    void operator()(json::Value result) const
    {
        json::Value response(json::TYPE_OBJECT);
        response.addMember("jsonrpc", "2.0");
        response.addMember("id", request_["id"]);
        response.addMember("result", result);
        callback_(response);
    }

private:
    json::Value request_;
    RpcDoneCallback callback_;
};

#endif