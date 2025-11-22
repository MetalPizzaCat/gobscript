# Goblin Script Helper 

Goblin script helper is a simple utility that uses lisp style syntax. This project was created to have a middle ground between shell scripting using bash and python, because I hate using bash scripting, but also don't want to deal with pip.

# Language

The language used for this project, named Pigeon(at least for the time being), is an interpreted functional LISP based language. Meaning that instead of writing `a = 2 + b`, you write `(= a (+ 2 b))`. The language itself is fairly similar 
See `language.md` for more

# Running code

## File mode

File mode is used to run scripts saved in the file and simply runs provided file and exits the program once it is done.

To run code simply use `gsh` app with `-i` argument to run code from file.

## Interactive mode

Interactive mode is used for running short code snippets and exploring various language features. It can be accessed by calling `gsh` with no arguments provided 

# Building

Note that the project is built around unix and  linux specifically, so while this project can run on windows and was tested on windows(built with msvc), functionality of `exec` is subpar on windows when build with msvc. Powershell appears to handle it fine, but vscode behaves strangely

## Linux 

This project uses cmake and does not rely on any external libraries so building should be pretty easy. The following uses shell script(ironic i know), however if you intend on development and use vscode it should automatically pickup on the project.

**IMPORTANT!** g++ and cmake are required.

Assuming you are now in the root directory of the project(folder containing `main.cpp` and `CMakeLists.txt`) do following
```sh
mkdir out
cmake -DCMAKE_BUILD_TYPE=Release -S ./ -B out
cmake --build out
```

If everything completed correctly the `out` folder will contain the `gsh` executable with can be used for running code


## Windows

This project uses cmake and does not rely on any external libraries so building should be pretty easy. The following was testing using Powershell, however if you intend on development and use vscode it should automatically pickup on the project.

**IMPORTANT!** You need visual studio and cmake to build. This was tested with visual studio community edition 2022. Alternatively you can build

```sh
mkdir out
cmake -DCMAKE_BUILD_TYPE=Release -S ./ -B out
cmake --build out
```