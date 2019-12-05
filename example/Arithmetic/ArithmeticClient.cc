/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-12-06 02:29
 * Last modified : 2019-12-06 02:47
 * Filename      : 
 * Description   : 
 **********************************************************/
#include "ArithmeticClientStub.h"
#include <iostream>
#include <EventLoop.h>

void getRPC(int a, int b, json::Value response, bool isError)
{
    if(!isError)
    {
        std::cout << std::endl;
        std::cout << a << " + " << b << " = " << response.getDouble() << std::endl;
    }
    else
    {
        std::cout << "response: "
            << response["message"].getStringView() << ": "
            << response["data"].getStringView() << "\n";
    }
    
}

void run(ArithmeticClientStub* client)
{
    double lsh = random() % 20;
    double rsh = random() % 50;
    client->Add(lsh, rsh, std::bind(&getRPC, lsh, rsh, _1, _2));
}

int main()
{
    EventLoop loop;
    sockaddr_in addr = fromPort(4396);
    ArithmeticClientStub client(&loop, addr);
    double a, b;
    loop.runEvery(2.0, std::bind(&run, &client));
    client.start();
    loop.loop();
}