//
// Created by raki on 19-10-11.
//

#ifndef CYCLONE_RPCSERVER_H
#define CYCLONE_RPCSERVER_H
#include "../Server.h"

class RpcServer:noncopyable
{
public:
    RpcServer(EventLoop* loop, const sockaddr_in&  listen, std::string name = "RpcServer", bool ReusePort = true);
    ~RpcServer();
    void onConnection(const ConnectionPtr& conn);

    void onMessage(const ConnectionPtr& conn, Buffer* buf);

    void onWriteComplete(const ConnectionPtr& conn);
private:

};


#endif //CYCLONE_RPCSERVER_H
