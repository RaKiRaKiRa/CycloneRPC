## 简介

CycloneRpc是以[Cyclone网络库](<https://github.com/RaKiRaKiRa/Cyclone>)为基础的异步多线程RPC框架，使用json进行序列化与反序列化，以[JSON-RPC2.0](https://www.jsonrpc.org/specification)作为传输协议。



## 结构

![](https://github.com/RaKiRaKiRa/CycloneRpc/blob/master/img/struct-img.png)

[Cyclone网络库](<https://github.com/RaKiRaKiRa/Cyclone>)位于框架底层, 向下封装Linux socket API, 向上提供消息回调接口.

 **json parser/generator**用于解析接收到的JSON object, 并生成需要发送的JSON object. 

**service/client stub**由程序stub generator根据提供的`spec.json`自动生成，用于调用用户定义的`service`或被用户通过`client`调用, 用户只要include相应的stub就可以接收/发起RPC.

