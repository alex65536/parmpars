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

* Generative parameters (see [#usage](the usage below))

* Preprocessor and constant substitution

* Independence from Testlib (though using ParmPars together with Testlib adds features like generative parameters)

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

### Parameters format

All the parameters should have form `<name>=<value>`. The last parameters may be a seed and contain only alphanumerical cha

### Basic things

* `parser.init(int argc, char *argv[])`: must be called at startup. Initializes the parser.  
* `initGenerator(int argc, char *argv[], int randomGeneratorVersion)`: Testlib required, runs both `registerGen` and `parser.init`.  
* `DECLARE(type, name, a...)`: declare a variable of type `type` and name `name` (which value is read from the parameters). Additional validation arguments can be supplied (see below). If the variable not found, exit with error.
* `DECLARE_D(type, name, def, a...)`: same as previous, but if the variable not found, use the default value.

### Parameter types

The following types are supported:

* Any integers
* `float`, `double`, `long double`
* `bool`, `char`
* `std::string`

### Validation

* For any integer and floating point types (including `bool` and `char`), range validation can be used. It can be used like this:

  ```cpp
  DECLARE(int, n, range(1, 100));
  ```
  
  If `n` is outside of range `[1; 100]`, the generator will throw the error.

* For string regex-based validation can be used. It looks in the following way:

  ```cpp
  DECLARE(string, s, "[a-z]{1,100}");
  ```

Validation also applies to `DECLARE_D`.

### Conditional compilation macros

You can use the macros to control the features of ParmPars. The following macros can be used:

* `PARMPARS_EXIT_ON_WARNING`: treats warnings as errors and exit the generator on each warning.
* `PARMPARS_USE_REGEX`: use `std::regex` to process regex validation. Disabled by default as not all compilers may support it. Also using STL regex increases compilation time.
  If `PARMPARS_USE_REGEX` is not defined, Testlib's regex implementation is used (if it was included)

### Generative parameters

This works only with Testlib. Currently, there are two classes for this:

* `GenRange<T>`: generates a number from the specified range.
* `GenRegex`: generates a string using regex.

How this works? Suppose you need to tell the generator that `n` must be from `1` to `10`, but want to give the ability for the generator to choose the exact value. You can write this:

```cpp
int main(int argc, char *argv[]) {
  initGenerator(argc, argv, 1);
  DECLARE_GEN(GenRange<int>, n);
  ...
```

Now run this generator as `./gen n=[1,10]`. It will read `[1,10]` as `GenRange<int>` and use it to generate a number from `1` to `10`, and then declare `int n` with this value. ParmPars accepts different syntaxes for generator ranges: round brackets are also acceptable, and `;` can be used as a separator instead of `,`.  It can be helpful, for example, when used with Polygon's stress tests.

You can also use a longer way:

```cpp
...
DECLARE(GenRange<int>, n);
int nGen = n.generate();
...
```
So, `DECLARE_GEN` is a shortcut to read `GenRange<T>` from the parameters and generate a value using one macro.

`DECLARE_GEN_D` does the same thing as `DECLARE_GEN`, but you can specify the default `GenRange<T>` which can be used for generating if the variable was not found.

`GenRange<T>` also supports range validation, it will validate not the generated value, but the range read from the input.

## Constant substitution

Tired of using something like `n=1000000` in all of your tests? Then, constant substitution if for you. ParmPars has a simple preprocessor. To define a constant, you can use something like

```cpp
param.define("n", 1000000);
```

When you `DECLARE` a parameter generator, ParmPars will use all the constants declared before.

The syntax of the preprocessor is quite simple. The constant should be used in the parameter as `@constName` or `@constName!`, the latter variant can be used to avoid confusion when the next character after the constant is a letter or number. If the variable name if empty, it will substitute to `@` character.

Examples:

Suppose we have these lines in `test.cpp`:

```cpp
...
param.define("c", "const");
DECLARE(string, s)
cout << "s = " << s << endl;
...
```

If we compile and run the executable, we will get:

* `test.exe s=@c` => s = const`
* `test.exe s=@c1` => Error, as ParmPars will try to search for constant `c1`
* `test.exe s=@c!1` => s = const1`
* `test.exe s=@` => `s = @`
* `test.exe s=@!c` => `s = @c`
* `test.exe s=@c+@c` => s = const+const`
* `test.exe s=@c!+@c!` => s = const+const`

See also `test_macros.cpp`.

## Need more examples

See `test.cpp`.

## Why not integrating with Testlib tightly?

Testlib is heavy enough, you may choose not to use it for your generator, but still use ParmPars. Also, you can use alternative libraries for generators, like [jngen](https://github.com/ifsmirnov/jngen), and use ParmPars with it.

## License

ParmPars is distributed under MIT license, like Testlib.
