/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-14 18:27
 * Last modified : 2019-11-23 20:55
 * Filename      : RpcService.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONE_RPCSERVICE_H
#define CYCLONE_RPCSERVICE_H
#include <Cyclone/net/base/noncopyable.h>
#include "common.h"
#include <string>
#include <unordered_map>
#include "common.h"
#include "ProcedureNotify.h"
#include "ProcedureRequest.h"

class RpcServer;

// 会在stubClass的构造函数中构造，然后被加入RpcServer的ServiceMap进行管理，每个Service中含有多个Procedure
class RpcService:noncopyable
{
public:
    typedef std::unique_ptr<ProcedureNotify> ProcedureNotifyPtr;
    typedef std::unique_ptr<ProcedureRequest> ProcedureRequestPtr;
    typedef std::unordered_map<std::string, ProcedureNotifyPtr> ProcedureNotifyMap;
    typedef std::unordered_map<std::string, ProcedureRequestPtr> ProcedureRequestMap;

    // 会在stubClass的[stubProcedureBindings]生成的代码中生成，在stubClass的构造函数中调用，将Procedure加入对应Service
    void addProcedureRequest(std::string methodName, ProcedureRequest* procedure);
    void addProcedureNotify(std::string methodName, ProcedureNotify* procedure);
    
    // RpcServer::handleSingleRequest中调用
    void callProcedureRequest(const std::string &method, const json::Value& request,const RpcDoneCallback& done, JSON_RPC_ERROR& err);
    // RpcServer::handleSingleNotify中调用
    void callProcedureNotify(const std::string &method, const json::Value& request, JSON_RPC_ERROR& err);
    
private:
    ProcedureNotifyMap procedureNotifyMap_;
    ProcedureRequestMap procedureRequestMap_;
};

#endif