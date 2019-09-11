# A VERY WIP custom scripting language lexer, parser and evaluator.
## What is this written in?
C++
## Will this ever go anywhere?
Probably not. Fun project I am working on towards the end of Summer 2019.
## What language does this parse?
One that I wanted to make for the fun of making it.  Most likely has no real world usecase.
### What is the langauge like?
It borrows alot of the its syntax from the great language that is Kotlin. Kotlin is a fantastic language to write in and it just makes sense. 
## Goals
1. Evaluate simple code like variable definitions and mathmatical operations on variables.
2. Evaluate functional code. Creating and calling functions, lamdas, etc.
3. Evaluate external functional code. Interfacing with the parent C++ application through vm function calls. 
4. Custom types like classes and structures.
5. Will have to figure that out if we every get here.
## Syntax
```

// External interface with parent code. Registered by VM.
extern fun print_int(arg : int);
extern fun print_generic(arg : Any);

// Creates a constant variable called some_val with the type int and the value of 35.
val some_val : int = 15 + 20;

// Creates a non constant variable called some_var with the implicit type int and value of 35.
var some_var = 15 + 20;

fun print_all(args : ...) {
    foreach(arg in args) {
        if(arg.type == int) {
            print_int(arg);
        }
        else if(arg.type == ...) {
            print_...(arg);
        }
        else {
            print_generic(arg);
        }
    }
}

print_all("h", 3, 1, 1, 0, " world");
// Prints : h3110 world

```