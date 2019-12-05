include_directories(
        jackson
        Cyclone_net
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/Cyclone/net
        ${PROJECT_SOURCE_DIR}/Cyclone/net/base
        ${PROJECT_SOURCE_DIR}/jackson/jackson
	${PROJECT_SOURCE_DIR}/CycloneRpc
	${PROJECT_SOURCE_DIR}/CycloneRpc/stub
	${PROJECT_SOURCE_DIR}/CycloneRpc/client)

add_definitions(-std=c++17)
add_definitions(-g)
add_subdirectory(Cyclone)
add_subdirectory(jackson)

add_subdirectory(CycloneRpc)
add_subdirectory(example)
