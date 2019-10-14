/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-14 18:36
 * Last modified : 2019-10-14 18:58
 * Filename      : 
 * Description   : common.h
 **********************************************************/
#ifndef CYCLONE_RPCCOMMON_H
#define CYCLONE_RPCCOMMON_H
#include <functional>
#include <memory>
#include "jackson/jackson/Value.h"
class RpcService;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

typedef std::unique_ptr<RpcService> RpcServeicPtr;
typedef std::function<void(json::Value response)> RpcDoneCallback;

typedef std::function<void(json::Value&, const RpcDoneCallback&)> ProcedureReturnCallback;
typedef std::function<void(json::Value&)> ProcedureNotifyCallback;

#endif