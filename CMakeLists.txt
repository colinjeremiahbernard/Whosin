cmake_minimum_required(VERSION 3.20)

project(Whosin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Crow CONFIG REQUIRED)

add_executable(whosin
    src/main.cpp
    src/services/RoomService.cpp
    src/services/IdGenerator.cpp
)

target_link_libraries(whosin
    PRIVATE
        Crow::Crow
)

target_compile_definitions(whosin
    PRIVATE
        _WIN32_WINNT=0x0A00
)