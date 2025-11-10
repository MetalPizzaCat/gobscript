# Goblin Script Helper 

Goblin script helper is a simple utility that uses lisp style syntax. This project was created to have a middle ground between shell scripting using bash and python, because I hate using bash scripting, but also don't want to deal with pip.

# Language

The language used for this project, named Pigeon(at least for the time being), is an interpreted functional LISP based language. Meaning that instead of writing `a = 2 + b`, you write `(= a (+ 2 b))`. The language itself is fairly similar 
See `language.md` for more

# Running code

To run code simply use `gsh` app with `-i` argument to run code from file.

# Building

Note that currently only linux is supported due to how `exec` is implemented. 

## Linux 

This project uses cmake and does not rely on any external libraries so building should be pretty easy. The following uses shell script(ironic i know), however if you intend on development and use vscode it should automatically pickup on the project.

Assuming you are now in the root directory of the project(folder containing `main.cpp` and `CMakeLists.txt`) do following
```sh
mkdir out
cmake -DCMAKE_BUILD_TYPE=Release -S ./ -B out
cmake --build out
```

If everything completed correctly the `out` folder will contain the `gsh` executable with can be used for running code
