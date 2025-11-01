cmake_minimum_required(VERSION 3.10.0)
project(gsh VERSION 0.1.0 LANGUAGES C CXX)

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

