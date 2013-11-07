# Funky

Funky is a library that contains uncommon utilities for C++11 programmers. Dependancies between different headers are minimized, so it's a take-what-you-need library.

Funky provides the following modules.

- `funky::Either<Left, Right>`, a haskell-inspired Either type: [source](include/funky/Either.hh), [docs](docs/Either.md).

## Requirements
Funky has no dependancies on any librarys other than a C++11 compliant compiler and standard library.


## Documentation
Funky has extensive documentation (in Markdown format) for all its modules in the docs folder.


## Usage
The best way to use funky is to copy the files you want into your project. Currently funky is a header-only library, but this may change in the future.

You can run the tests by using `make run-tests`. This is the default target for the makefile, so just `make` will work too.

## License

Funky is distributed under the terms of the Boost Software License. See the [license file](LICENSE.md) for details.
