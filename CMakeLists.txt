include_directories(
        jackson
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR})
add_definitions(-std=c++17)
add_definitions(-g)
add_subdirectory(jackson)
add_subdirectory(stub)
