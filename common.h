/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-14 18:36
 * Last modified : 2019-10-18 16:19
 * Filename      : 
 * Description   : common.h
 **********************************************************/
#ifndef CYCLONE_RPCCOMMON_H
#define CYCLONE_RPCCOMMON_H
#include <functional>
#include <memory>
#include "jackson/jackson/Value.h"

class RpcService;
template<typename ProcedureType> class Procedure;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

typedef std::unique_ptr<RpcService> RpcServeicPtr;
typedef std::function<void(const json::Value&)> RpcDoneCallback;



typedef std::function<void(json::Value&, const RpcDoneCallback&)> ProcedureRequestCallback;
typedef std::function<void(json::Value&)> ProcedureNotifyCallback;
typedef Procedure<ProcedureRequestCallback> ProcedureRequest;
typedef Procedure<ProcedureNotifyCallback> ProcedureNofity;
#endif