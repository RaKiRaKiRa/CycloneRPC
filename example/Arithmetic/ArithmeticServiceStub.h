
        #ifndef CYCLONERPC_ARITHMETICSERVICESTUB_H
        #define CYCLONERPC_ARITHMETICSERVICESTUB_H
        #include <jackson/jackson/Value.h>
        #include <CycloneRpc/RpcService.h>
        #include <CycloneRpc/RpcServer.h>

        class ArithmeticService;

        template <typename userClassName>
        class ArithmeticServiceStub: noncopyable
        {
        protected:
            explicit ArithmeticServiceStub(RpcServer &server)
            {
                RpcService* service = new RpcService;

                //typedef std::function<void (const json::Value &, const RpcDoneCallback &)> ProcedureRequestCallback
                service->addProcedureRequest("Add", 
                        new ProcedureRequest(std::bind(&ArithmeticServiceStub::AddStub, this, _1, _2)
                        , "lhs", json::TYPE_DOUBLE, "rhs", json::TYPE_DOUBLE)
                        );


                service->addProcedureRequest("Sub", 
                        new ProcedureRequest(std::bind(&ArithmeticServiceStub::SubStub, this, _1, _2)
                        , "lhs", json::TYPE_DOUBLE, "rhs", json::TYPE_DOUBLE)
                        );


                service->addProcedureRequest("Mul", 
                        new ProcedureRequest(std::bind(&ArithmeticServiceStub::MulStub, this, _1, _2)
                        , "lhs", json::TYPE_DOUBLE, "rhs", json::TYPE_DOUBLE)
                        );


                service->addProcedureRequest("Div", 
                        new ProcedureRequest(std::bind(&ArithmeticServiceStub::DivStub, this, _1, _2)
                        , "lhs", json::TYPE_DOUBLE, "rhs", json::TYPE_DOUBLE)
                        );



                server.addService("Arithmetic", service);
            }
        private:

            void AddStub(const json::Value& request, const RpcDoneCallback& done)
            {
                json::Value params = request["params"];
                if(params.isArray())
                {
auto lhs = params[0].getDouble();
auto rhs = params[1].getDouble();

                    service().Add(lhs, rhs,  UserDoneCallback(request, done));
                }
                else
                {
auto lhs = params["lhs"].getDouble();
auto rhs = params["rhs"].getDouble();

                    service().Add(lhs, rhs,  UserDoneCallback(request, done));
                }
            }


            void SubStub(const json::Value& request, const RpcDoneCallback& done)
            {
                json::Value params = request["params"];
                if(params.isArray())
                {
auto lhs = params[0].getDouble();
auto rhs = params[1].getDouble();

                    service().Sub(lhs, rhs,  UserDoneCallback(request, done));
                }
                else
                {
auto lhs = params["lhs"].getDouble();
auto rhs = params["rhs"].getDouble();

                    service().Sub(lhs, rhs,  UserDoneCallback(request, done));
                }
            }


            void MulStub(const json::Value& request, const RpcDoneCallback& done)
            {
                json::Value params = request["params"];
                if(params.isArray())
                {
auto lhs = params[0].getDouble();
auto rhs = params[1].getDouble();

                    service().Mul(lhs, rhs,  UserDoneCallback(request, done));
                }
                else
                {
auto lhs = params["lhs"].getDouble();
auto rhs = params["rhs"].getDouble();

                    service().Mul(lhs, rhs,  UserDoneCallback(request, done));
                }
            }


            void DivStub(const json::Value& request, const RpcDoneCallback& done)
            {
                json::Value params = request["params"];
                if(params.isArray())
                {
auto lhs = params[0].getDouble();
auto rhs = params[1].getDouble();

                    service().Div(lhs, rhs,  UserDoneCallback(request, done));
                }
                else
                {
auto lhs = params["lhs"].getDouble();
auto rhs = params["rhs"].getDouble();

                    service().Div(lhs, rhs,  UserDoneCallback(request, done));
                }
            }



            userClassName& service()
            {
                return static_cast<userClassName&>(*this);
            }
        };
        #endif
    