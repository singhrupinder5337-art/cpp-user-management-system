cmake_minimum_required(VERSION 3.20)

project(UserAccountSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.3.4
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(Crow)

add_executable(server main.cpp)

target_link_libraries(server PRIVATE
    Crow::Crow
    ws2_32
    mswsock
)