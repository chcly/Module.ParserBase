# ParserBase

Is a base class library for deriving custom parsers.

It contains classes that are common with most language parsers.

It is up to the derived library to define the grammar and language specifics.

+ [Example](EXAMPLE.md) - Shows usage for a key value parser.

## Testing

The testing directory is setup to work with [googletest](https://github.com/google/googletest).
It contains the initial setup for testing this library as a standalone module using GitHub actions.

## Building

It builds with cmake, and it has been tested on Windows and Unix platforms using
the Visual Studio an Unix MakeFile cmake generators.

### Building with CMake and Make

```sh
mkdir build
cd build
cmake .. -DParserBase_BUILD_TEST=ON -DParserBase_AUTO_RUN_TEST=ON
make
```

### Optional defines

| Option                        | Description                                          | Default |
| :---------------------------- | :--------------------------------------------------- | :-----: |
| ParserBase_BUILD_TEST         | Build the unit test program.                         |   ON    |
| ParserBase_AUTO_RUN_TEST      | Automatically run the test program.                  |   OFF   |
| ParserBase_USE_STATIC_RUNTIME | Build with the MultiThreaded(Debug) runtime library. |   ON    |
