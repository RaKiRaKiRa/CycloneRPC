add_definitions(-std=c++17)
add_executable(CycloneStub
    StubException.h
    StubGenerator.h
    StubGenerator.cc
    ServerStubGenerator.h
    ServerStubGenerator.cc
    main.cc)

target_link_libraries(CycloneStub jackson)
