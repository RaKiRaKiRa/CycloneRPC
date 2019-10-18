/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-14 18:27
 * Last modified : 2019-10-18 21:11
 * Filename      : RpcService.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONE_RPCSERVICE_H
#define CYCLONE_RPCSERVICE_H
#include "Cyclone/net/base/noncopyable.h"
#include "common.h"
#include <string>
#include <unordered_map>
#include "common.h"

class RpcServer;
template<typename ProcedureType> class Procedure;

// 会在stubClass的构造函数中构造，然后被加入RpcServer的ServiceMap进行管理，每个Service中含有多个Procedure
class RpcService:noncopyable
{
    typedef std::unique_ptr<ProcedureNofity> ProcedureNofityPtr;
    typedef std::unique_ptr<ProcedureRequest> ProcedureRequestPtr;
    typedef std::unordered_map<std::string, ProcedureNofityPtr> ProcedureNofityMap;
    typedef std::unordered_map<std::string, ProcedureRequestPtr> ProcedureRequestMap;
public:
    // 会在stubClass的[stubProcedureBindings]生成的代码中生成，在stubClass的构造函数中调用，将Procedure加入对应Service
    void addProcedureRequest(std::string methodName, ProcedureNofity* procedure);
    void addProcedureNotify(std::string methodName, ProcedureNofity* procedure);
    
    // RpcServer::handleSingleRequest中调用
    void callProcedureRequest(const std::string &method, const json::Value& request, const RpcDoneCallback& done);
    // RpcServer::handleSingleNotify中调用
    void callProcedureNotify(const std::string &method, const json::Value& request);
    
private:
    ProcedureNofityMap procedureNofityList_;
    ProcedureRequestMap procedureRequestList_;
};

#endif