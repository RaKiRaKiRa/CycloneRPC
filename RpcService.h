/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-14 18:27
 * Last modified : 2019-10-14 19:02
 * Filename      : RpcService.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONE_RPCSERVICE_H
#define CYCLONE_RPCSERVICE_H
#include "Cyclone/net/base/noncopyable.h"
#include "common.h"
#include <string>

class RpcServer;
class Procedure;

class RpcService:noncopyable
{
public:
    void callProcedureRequest(const std::string &method, const json::Value& request, RpcDoneCallback done);
    void callProcedureNotify(const std::string &method, const json::Value& request);
    
private:


};

#endif