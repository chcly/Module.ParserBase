# ParserBase

Is a base class library for deriving custom parsers.

It contains classes that are common with most language parsers.

It is up to the derived library to define the grammar and language specifics.

## Testing

The Test directory is setup to work with [googletest](https://github.com/google/googletest).

## Building

Building with CMake and Make.

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

