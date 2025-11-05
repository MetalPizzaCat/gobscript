# Pigeon language

Pigeon language is a LISP based functional interpreted language designed to be a middle point between bash and python. The language is currently in development and certain features could be change and new features can be added.

Why is it called pigeon? I needed a name for it. Most likely will rename *it* to GobScript, but for now it will remain thus.

## Type of language

Although this language is based on a functional paradigm, it is not itself a functional language. Not a true one anyway. It was designed around the idea of a functional language, although more so as a consequence of using common lisp as inspiration more than anything, but it does not try to strictly adhere to functional language principles.  I don't intend to create Prolog, no matter how funny it would be. There are certain aspects that are influenced by the current implementation and ideas, such as lack of flow control operations such as `break` or `return` which require functions to be interrupted rather that exit gracefully.

# Basics

## Types
There are 4 types of data that can be used
* Integer - represented by 64bit signed integer
* String - Simple string represented by `std::string`
* Array - Simple dynamic array represented by `std::vector`
* Function reference - Pointer to either a function declared in the script or an externally bound function


## Syntax

This language follows common lisp inspired syntax similar to polish notation. Operations are written as `(operation arg1 arg2 argn)`. Expressions need to be wrapped in brackets and all expressions return values. Compound actions such as loops and sequences return last value in the sequence.

### Expressions

As stated previously all expressions follow `(op arg arg)` structure, although the specifics will vary depending on the operation itself. Expressions always return some value regardless of what they are. An empty expression `()` will be converted to `0` during parsing. Moreover, even though this language does share a lot in common with common lisp, it doesn't use the exact same syntax. Comments are written differently, certain aspects such as macros are not present and syntax in general is different. I was not referencing common lisp spec when designing this so a lot of things could be incompatible.

### Strings

Because this was designed around shell scripting and automating commands, strings are a bit unusual. Although there is the expected way to write strings using `"such as this"`, to make it easier to use `exec` command with arguments a raw non recognized sequence of characters will be treated as a string. For instance `(print hello)` will give same result as `(print hello)`. This way you can simple write `(exec ffmpeg -i file.mkv file.mp4)` instead of having to write  `(exec "ffmpeg" "-i" "file.mkv" "file.mp4")` However, in spite of this, I recommend using quoted strings outside of this specific situation, to avoid confusion.

There is also a way to write characters similar to c characters, however there is no special considerations for this type and it will be treated as integer. So `'l'` will simply return `108`(decimal). Whenever printing is needed simply use strings. However this could be useful for assigning operations, for instance `(set $str 0 'l')` will change first character of the string to `l`. Same could be accomplished by passing a string. All but the first character will be ignored. This is simply to make working with strings easier.

Operations such as `str[i]` can be performed using `at` operator, although it returns a string containing the character, not the integer value. For example, with `$str` being equal to `""Hello world"` calling `(at $str 2)` will return a *new* string containing only value "e". 

### Sequencing

Unlike most other languages, even lisp inspired ones such as the one used in Sierra Creative Interpreter, expression bodies can only contain one expression. So for instance while in python you can write several expressions and these actions will execute one after another. Example: 

```py
if value:
    print("hi")
    print("hello")
    print("how are you")
else:
    print("fascinating")
    print(value)
```
But in this language due to it's structure and to make it easier to know which block does what, it is limited to one expression per block

```lsp
    (if value (retunvalue1) else (returnvalue2))
```
If you need more than one expression on the block, executed in a *sequence* then you can use sequence operator(similar to `progn` in common lisp). It simply executes all expressions in top down order and then returns values of the last expression. Or 0 if no expression was executed. So similar code in pigeon will be:
```lsp
(if ($value) 
    (seq
        (print "hi")
        (print "hello")
        (print "how are you")
    )
    else
    (seq 
        (print "hi")
        (print $value)
    )
)
```
It should be noted that the return values of that code would be `0` because `print` returns 0

### Loops

Loops will run for as long as conditions are true and return the result of last expression executed. Note that much like everything to perform more than one action in the loop body a `seq` expression is required. Also note that there are no restrictions on the expressions part of the loop. You can write logic with sequences, function calls or even others loops.

Something important to remember is that the language lacks `break` and `continue` statements. Although I am aware that common lisp, sci etc. have some form of break, i don't plan on adding it, at least not to this iteration of the language. Adding it would require far too many changes to the interpreter and also to the language itself. Although it is possible and i might add it in the future iteration of the language(one that most likely won't be in this project), I will keep this as is.

#### For loop

For loops are meant to be like c loops. In fact, they are currently implemented using a `for` loop.  They have initial expression, condition and end-of-iteration expression. They are evaluated in the following order:

* Initial expression
* Condition
* Expression
* End of iteration expression

Variables can not be declared within initialization expression, like they can be in modern c, so if you want to have a variable to iterate with you have to declare it using a variable block. For example, a simple loop that goes from 0 to 100 and prints every number would look like this:

```lsp
(let ((i 0))
    (for ((= i 0) (< $i 100) (+= i 1))
        (print $i)
    )
)

```

Because value `i` is initialized to 0 already, `(= i 0)` can be omitted and `()` can be written instead. Because of how the expression system works this loop could be used itself to generate a value for an expression. For example

```lsp
(let ((foob 0))
    (seq 
        (= foob 
            (let ((i 0))
                (for ((= i 0) (< $i 100) (+= i 1))
                    (print $i)
                )
            )
        )
        (print $foob)
    )
)
```
which will both assign `foob` to 100 and print each value. This is technically achievable in other languages with use of functions, but made a lot easier here.

#### While loop

While loop functions similarly to for loop, expect that it lacks init and iteration statements. Example of a while loop that iterates until function returns a value that matches the goal:

```lsp
(while (!= $val $goal)
    (= val (call :advance $val))
)
```

### Arrays

Array is a special type that holds more than one value and it can be dynamically resized and contain any data type. To create an array with default values use `array` operation which takes in any number of arguments and returns an array with those values. For example `(array 1 2 "hello world")` is used to create an array with values 1, 2, "hello world". To create an empty array a simple call to `(array)` with no arguments will work. Because it expects any amount of arguments it will work with no arguments as well. Both `set` and `at` operations are applicable to arrays. 

Arrays in this language are closer to vector in c++ or List in c# and not slices in go. This means that operations that change values in the array will change the value you passed in, rather than create a copy.

For instance to add value to an array `append` is used, The following example shows array being changed by adding a new value:

```lsp
(let ((arr (array 1 "hi" 3)))
    (seq
        (print $arr)
        (append $arr 420)
        (print $arr)
    )
)
```
This will produce
```
[1,hi,3]	
[1,hi,3,420]
```
as the output.

The original value of the array was modified, rather than a copy created. 
Arrays can be processed using various iteration functions, for instance through using `filter` operation you can create a *new* array that returns . For example the following code will print `[2,4,6]`:

```lsp
(func even (val) (not (% $val 2)))
(print (filter (array 1 2 3 4 5 6) :even))
```

Alternatively a simple loop could be used to achieve the same goal. The following code produces same result, except that it requires more code to be written

```lsp
(print 
    (let ((i 0) (arr (array 1 2 3 4 5 6)) (res (array)))
        (seq 
            (for (() ( < $i (len $arr)) (+= $i 1))
                (if (% (at $arr $i ) 2) 
                        () 
                 else
                        (append $res (at $arr $i))
                )
            )
            ($res)
        )
    )
)
```

## Variables
Variables in the language can be accessed by using `$variable` syntax. Unlike most languages, variables have be to be declared in special blocks, which also describes the lifespan of the variable. 

```lsp
(let (  
        (variable_name initial_value)
        (variable_name initial_value)
    )
    (action)
)
```
Note that during declaration `$` is omitted, so for example a block that declares and then prints variable `example` will look like this. 

```lsp
(let ((example "example"))
    (print $example)
)
```
This is mostly to make it less tedious to use and because it is clear that it is not a usual expression. All other places which can use variables do require `$`, even if no other expression can be used in place of it, such as assignment operation. Assignment uses c style operators and even supports the compound assignment operators such as `+=`.  For example, to set value of the variable `example` you can do this: 

```lsp
(let ((example "example"))
    (= $example "not example")
)
```
Variables are accessible in any block nested withing the variable block that declared them and can be shadowed. For instance the following code

```lsp
(let ((example "example"))
    (let ((example "dubious"))
        (= $example "not example")
        (print $example)
    )
    (print $example)
)
```

Produces

```
not example	
example	
```

as it's output. Although name shadowing isn't encouraged, it can be used whenever useful.

# Functions

Functions are a simple blocks of code that help with avoiding code repetition. Functions work similarly to functions in a language like python, although they are closer to pure functions. Few key things about about function in pigeon is that all values are passed-by-copy into the function, so it is not possible to directly modify the original value. This doesn't perform a deep copy however, so an array or a string are passed by reference. Modifying them will result in the original value being modified. 

Functions can be declared anywhere, however they do have a global scope. Moreover, function declaration *itself* is an action. This means that you can conditionally declare functions or even attempt to declare them in a loop, although this will end up just overriding previous definition, because they are stored using a `name-func` pair dictionary.

To declare a function use `func` operation which follows `(func name (arg arg arg) (body))` structure. The `name` is a constant string which is only evaluated during parsing, so it can't be dynamically generated using an expression.  For example a function that doubles it's argument would be:

```lsp
(func double (val) (* $val 2))
```

Argument field can be left empty, in case function doesn't require arguments

```lsp
(func pi () (5))
```

To access a function you can use `:` operator. `:` followed by function name will return a callable pointer to a given function. This is required for the `call` expression which uses a callable object for its operation. So for example to call `double` function from previous example the following code can be used:

```lsp
    (print (call :double 1))
```

Do also note that the c++ functions bound to the state of the interpeter can be accessed similarly `(call :print 1 2 "hello")` and `(print 1 2 "hello")` produce the same result. Furthermore the `:` operator simply returns a function reference value, so if you wish you can store them in an array or other variable and call them this way, for example:

```lsp
(func f1 (val) (print (+  $val 2 )))
(func f2 (val) (print (*  $val 2 )))
(func f3 (val) (print (/  $val 2 )))

(let ((funcs (array :f1 :f2 :f3 )) (i 0))
    (for (()( < $i (len $funcs)) (+= $i 1))
        (call (at $funcs $i) 500)
    )
)
```

This code will execute each function in order.


It is important to note that the arguments in the function bodies are just usual variables themselves so they can be assigned to. *Assignment* won't affect the original value and this mechanic is similar to how you can modify arguments as variables in c. For example this code modifies value of `val`:

```lsp
(func double (val) 
    (seq 
        (print $val)
        (*= $val 2)
        (print $val)
        ($val)
    )
)

(call :double 4)
```

This causes no errors and prints 

```
4
8
```

Function declaration returns pointer to the function itself, which can be used without having to manually write down names. 


# Interpretation

This language uses a bit of an usual interpretation, although it does make expanding and making language easier.

## Execution method

Instead of being compiled into machine code or bytecode for a virtual interpreter it converts code into `Action` class which represents an expression and creates a tree of actions which it then executes. This does technically introduce the limit on the expression length, as the amount of actions you can nest will be limited by the call stack size, however the stack size is normally large enough to accommodate it. An example of a  language with a similar method of execution is Scratch, although because of the nature of that language, it uses a different method of ordering and calling functions, but general idea remains the same.

So for example expression `(+ 2 $b)` will be parsed into a tree like this

```
BinaryOperationAction { GetConstNumberAction VariableAccessAction }
```

And then be executed recursively.

I have used this method before for a different, visual programming language, and it has proven viable. However major downside of this approach is increase ram usage, because instead of storing a sequence of bytes with their arguments, we have to store whole objects. Furthermore this project was designed in a way that accommodates the possibility of converting it into a bytecode, so it is possible to reuse this project to work differently.

## Garbage collection

This language uses garbage collection and takes the simplest approach of reference counting. Each "pointer object", such as String or Array, will have a reference counter which is incremented each time it is assigned or put into an array. Once the variable that uses it goes out of scope(leaves the variable block or function body), reference is decreased and garbage collector is called. Note that to avoid accidentally freeing memory withing nested c++ call, references are increased before calling c++ functions as well. Garbage collector is simply called whenever variable block is removed, which then checks all reference numbers.

The only exception for decreasing reference count for variables of a block is for the variable that is returned at the end of the block. This variable has its reference count increased before calling garbage collector, which then is decreased before value is actually passed to the parent action. This does mean that the value will be freed during the next gc call, so it doesn't become an indestructible object.

# Expanding language

