SET(RPCSRC
RpcServer.cc
RpcService.cc
ProcedureNotify.cc
ProcedureRequest.cc
RpcClient.cc
)

add_definitions(-std=c++17)
add_definitions(-g)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/stub
	${PROJECT_SOURCE_DIR}/client)

add_subdirectory(stub)

add_library(Cyclone_Rpc ${RPCSRC})
target_link_libraries(Cyclone_Rpc jackson Cyclone_net)


