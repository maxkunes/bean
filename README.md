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
It borrows alot of the its syntax from the C family of languages and also from Kotlin (the best JVM based language!).
## Goals
- [x] Evaluate simple code like variable definitions and mathmatical operations on variables.
- [x] Evaluate functional code. Creating and calling functions.
- [x] Evaluate external functional code. Interfacing with the parent C++ application through vm function calls. \* 
- [ ] Custom types like classes and structures.
- [ ] Will have to figure that out if we every get here.
- [ ] Integrate with LLVM or transcode to C++. JIT compilation would be nice.

\* Removed temporarily but essentially completed.

## What can it do right now?
I could try to write up a complicated paragraph on what exactly it can do, however, I will instead just show you a test script that utilizes most of what the language offers right now.

```
fun evaluate_complex_expression {
    var expr_value = (6.5 * 2 + 8.5) * (4 / 2) ^ 2;
    return expr_value;
}

// returns to c++ a bean_object_double instance with a value of 86.0
return evaluate_complex_expression();
```

## How to use Bean

```
#include <bean_vm.hpp>

...

try {
    auto vm = bean_vm();
    std::cout << vm.eval_result("var x = (1 + 2);")->as_integer() << std::endl;
}
catch (const std::exception& e)
{
	std::cout << "Exception occured while parsing or evaluating script." << std::endl;
	std::cout << e.what() << std::endl;
}
```

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

## Tests

All relevent tests can be found in the [Tests.cpp](https://github.com/maxkunes/bean/blob/master/tests.cpp) file and the wonderful [Catch2](https://github.com/catchorg/Catch2) library is used for unit testing.
