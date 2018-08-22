# ParmPars

## Why can I need it?

If you write generators using [testlib](https://github.com/MikeMirzayanov/testlib), you can probable feel the inconvenience to write as described in the official documentation:

```cpp
int main(int argc, char *argv[]) {
  registerGen(argc, argv, 1);
  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  ...
```

And, if your generator has too many parameters, you should remember what each parameter is used for.

ParmPars makes it easier to parse command line parameters. Consider this example:

```cpp
int main(int argc, char *argv[]) {
 initGenerator(argv[1]);
 DECLARE(int, n, range(1, 1'000'000));
 DECLARE(int, m, range(1, n));
 ...
```

The generator can be invoked using, e.g., the following line: `./gen n=10 m=3`. This way of invoking and writing generator looks more clean and less error-prone. You won't forget which parameter is used for what. And using `DECLARE_D`, you can add default values for your parameters, so decreasing the amount of them in the command line.

## Features

* Error reporting (e. g. when a parameter was passed and not used in the program)

* Parameter validation

* Generative parameters (see (the usage below)[#usage])

* Support for adding constants and variable substitution (planned, but not implemented)

* Independence from Testlib (though using ParmPars together with Testlib adds features like generatrive parameters)

## Disadvantages

* Still alpha, may contain bugs

* May compile slowly

* Too many templates

## Why so many templates?

They allow to extend the library for the new types and validations easily. They make the architecture cleaner and prevent from copy-pasting such methods as getting the value of the variable for different types.

## Adding to your generator

To use ParmPars, just 

```cpp
#include "parmpars.hpp"
```

If you also use Testlib, place this include after `#include "testlib.h"`.

You may add some macros to tweak the behaviour of ParmPars.

## Usage

TODO
