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
- [x] Evaluate external functional code. Interfacing with the parent C++ application through vm function calls.
- [ ] Custom types like classes and structures.
- [ ] Will have to figure that out if we every get here.
- [ ] Integrate with LLVM or transcode to C++. JIT compilation would be nice.

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

### AST Visualization for the script above

![ast](https://i.imgur.com/Cs1e2ta.png)
it is not very pretty, but it is functional.

Bean also supports seemless C++ function binding through type deduction templates. Currently, raw function binding and std::function are supported with plain lamdas to come later. Right now, only two basic types can be bound, Integer and Double. This is because these are currently the only integral Bean types. Binding a new type takes a few lines of boilerplate code so when new integral Bean types are added I will also bind them.

Simple example:

```

std::function<std::int32_t(std::int32_t, double)> add_int_double = [&](std::int32_t a, double b) {
	return std::int32_t(a + b);
};

You can also use a standard function to bind with the same binding code as std::function:
std::int32_t add_int_double(std::int32_t a, double b) {
	return std::int32_t(a + b);
};


auto vm = bean_vm();
vm.bind_function("add_int_double", do_something);

auto res = vm.eval_result("add_int_double(1, 2)");

REQUIRE(res->as_int() == 3);

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
