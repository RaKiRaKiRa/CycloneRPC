/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-14 18:36
 * Last modified : 2019-10-20 15:08
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

#endif