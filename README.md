# ParserBase

Parser base is a base class library for deriving custom hand written domain specific
languages, or simple scripting languages.

It contains the Parser, Scanner, and Token classes that are common with most
standard language parsers. Its up to the derived library to define the grammar and
language specifics.

This library depends on the [Utility](https://github.com/CharlesCarley/Internal.Utils) library for data structures and type declarations.

## Sample (Config Parser)

This sample walks through building a minimal key/value parse API.
The parsed source code should resemble the following fragment.

```txt
# line comment
a = 1; 
b = 2; 
```

The grammar for it is defined by the following fragment.

```txt
id      = [a-z]+
value   = [0-9]+

<assignment> ::= <key_value> <assignment>
                 | /*empty*/
<key_value>  ::= id '=' value ';'
```

The class TokenBase is used to relay information from the input stream in the scanner to the parser.
Which then, the parser matches rules from the supplied tokens to store structured key values into a map for later lookup.

It defines the meaning of a token using two types.
An integer type `int8_t` which is an enumerated lookup code.
As well as an optional index `size_t` which points to a cached string or integer value.

For this sample the following tokens are defined.

```c++
enum TokenId
{
    TOK_ERROR = -2,
    TOK_EOF,             // no more characters in the stream
    TOK_NULL,            // undefined value
    TOK_KEY,             // a character sequence [a-z]+
    TOK_VALUE,           // a character sequence [0-9]+
    TOK_EQ        = '=', // character =
    TOK_SEMICOLON = ';'  // character ;
};
```


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
