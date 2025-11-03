cmake_minimum_required(VERSION 3.10.0)
project(gsh VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_compile_definitions(APP_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_compile_definitions(APP_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_compile_definitions(APP_VERSION_PATCH=${PROJECT_VERSION_PATCH})

add_executable(gsh main.cpp
    Pigeon/Action.hpp
    Pigeon/Action.cpp
    Pigeon/Memory.hpp
    Pigeon/Memory.cpp
    Pigeon/Parser.hpp
    Pigeon/Parser.cpp
    Pigeon/Value.hpp
    Pigeon/Value.cpp
    Pigeon/State.hpp
    Pigeon/State.cpp
    Pigeon/Operation.hpp
    Pigeon/Error.hpp
    Pigeon/Error.cpp
    Pigeon/Array.hpp
    Pigeon/Array.cpp
    Pigeon/Function.hpp
    Pigeon/Function.cpp
    Pigeon/StandardFunctions.hpp
    GobScriptHelper/StandardFunctions.hpp
    GobScriptHelper/StandardFunctions.cpp
    Pigeon/Execution.hpp
    Pigeon/Execution.cpp
)

