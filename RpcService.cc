/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-20 16:45
 * Last modified : 2019-11-16 01:28
 * Filename      : RpcService.cc
 * Description   : 
 **********************************************************/

#include "RpcService.h"

void RpcService::addProcedureRequest(std::string methodName, ProcedureRequest* procedure)
{
    assert(procedureRequestMap_.find(methodName) == procedureRequestMap_.end());
    procedureRequestMap_.emplace(methodName, procedure);
}
void RpcService::addProcedureNotify(std::string methodName, ProcedureNotify* procedure)
{
    assert(procedureNotifyMap_.find(methodName) == procedureNotifyMap_.end());
    procedureNotifyMap_.emplace(methodName, procedure);
}

// RpcServer::handleSingleRequest中调用
void RpcService::callProcedureRequest(const std::string &method, const json::Value& request,const RpcDoneCallback& done, JSON_RPC_ERROR& err)
{
    ProcedureRequestMap::iterator it = procedureRequestMap_.find(method);
    if(it == procedureRequestMap_.end())
    {
        err = METHOD_NOT_FOUND;
        return;
    }
    it->second->invoke(request, err, done);
}
// RpcServer::handleSingleNotify中调用
void RpcService::callProcedureNotify(const std::string &method, const json::Value& request, JSON_RPC_ERROR& err)
{
    ProcedureNotifyMap::iterator it = procedureNotifyMap_.find(method);
    if(it == procedureNotifyMap_.end())
    {
        err = METHOD_NOT_FOUND;
        return;
    }
    it->second->invoke(request, err);
}