# A VERY WIP custom scripting language lexer, parser and evaluator.
## What is this written in?
C++, targeting C++17
## Will this ever go anywhere?
Probably not. Fun project I am working on starting late summer 2019.
## Purpose
I wanted to write a custom programming language. Not because this is or will be useful as an actual progamming language, but because I wanted to figure out how to make one.


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
6. Integrate with LLVM or transcode to C++. JIT compilation would be nice.

## What can it do right now?
Right now it can interpret and run mathmatical expressions. From `2 + 2` to `(4 + 5 * (2 + 3) * 5) * 5 * (4/3)` and a few operators in between.
This is important as the code to parse an expression due to operator precedence is built, working and tested which will be used by the ast builder for the language.
The precedence followed is PEMDAS left to right. Future operator precedence for non mathmatical operations will likely be based off the C/C++ standard. Examples of those types of operators would be the xor and shift operators.
1. Parentheses
2. Exponents (Experimental and likely to be removed ^ and pow operators)
3. Muliplication and Division (Precedence is left-right)
4. Addition and Subtraction (Precedence is left-right)


## Syntax
```

// External interface with parent code. Registered by VM.
extern fun print_int(arg : int);
extern fun print_generic(arg : Any);

// Creates a constant variable called some_val with the type int and the value of 860.
val some_val : int = (4 + 5 * (2 + 3) * 5) * 5 * (4/3);

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