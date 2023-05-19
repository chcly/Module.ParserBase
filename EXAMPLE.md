# Sample (Config Parser)

This sample walks through building a minimal key/value parse API.

The parsed source code should resemble something like the following.

```txt
# line comment
a = 1; 
b = 2; 
```

The keys will be defined as the set of one or more lowercase characters a through z.
And, the values will be defined as the set of one or more digits 0 through 9.
Internally, the value will be stored as an integer.

Using this description, the grammar is defined by the following fragment.

```txt
id      = [a-z]+
value   = [0-9]+
<assignment> ::= <key_value> <assignment>
                 | // empty
<key_value>  ::= id '=' value ';'
```

## Translating it to code

The TokenBase class is used to relay information from the input stream to the parser.
It defines the meaning of a token using two types. An integer type `int8_t` which is assigned as an enumerated lookup code.
As well as an optional index `size_t` which points to a cached string or integer value.
The cached values are stored in the scanner via the 'save' method, and they are retrieved via the 'string' and 'integer' methods.

Each time the scanner matches a token, it should save any relevant data and codes to the supplied token, then immediately return from `scan`.

The parser is setup to scan tokens automatically from a cursor position.

The `token(idx)' method uses the idx parameter as an offset from the cursor.
So if the cursor is at 0 and idx is 3, then 4 tokens will be read, so that tokens
[0-3] can be retrieved.

Then once a rule is determined to be valid, the 'advanceCursor' method moves the cursor forward. This way matching can assume that the next rule to test will start at index 0. To guarantee this assumption, the parse should either match or be in error (100% or fail). The scanner should use the `syntaxError` method to report invalid state, and the parser should use the `error` method.

The following code implements this parser, which may also be found in the stand alone test for this repository.

```c++
enum TokenId
{
    TOK_ERROR = -2,
    TOK_EOF,              // no more characters in the stream
    TOK_NULL,             // undefined value
    TOK_KEY,              // a character sequence [a-z]+
    TOK_VALUE,            // a character sequence [0-9]+
    TOK_EQ        = '=',  // character =
    TOK_SEMICOLON = ';'   // character ;
};

class ConfigScanner final : public ScannerBase
{
public:
    void scanInt(TokenBase& tok)
    {
        String dest;
        int    ch = _stream->get();

        while (isDecimal(ch))
        {
            dest.push_back((char)ch);
            ch = _stream->get();

            if (ch <= 0)
                syntaxError("unexpected end of file");
        }
        _stream->putback((char)ch);

        // save it to the cache and set it's index
        tok.setIndex(save(Char::toUint32(dest)));
        tok.setType(TOK_VALUE);
    }

    void scanId(TokenBase& tok)
    {
        String dest;

        int ch = _stream->get();
        while (isLower(ch))
        {
            dest.push_back((char)ch);
            ch = _stream->get();

            if (ch <= 0)
                syntaxError("unexpected end of file");
        }
        _stream->putback((char)ch);


        // save it to the cache and set it's index
        tok.setIndex(save(dest));
        tok.setType(TOK_KEY);
    }


    void scan(TokenBase& tok) override
    {
        if (_stream == nullptr)
            syntaxError("no supplied stream");

        tok.clear();
        tok.setType(TOK_NULL);

        int ch;
        while ((ch = _stream->get()) > 0)
        {
            tok.setLine(_line);
            switch (ch)
            {
            case '#':
                scanLineComment();
                break;
            case '=':
                tok.setType(TOK_EQ);
                return;
            case ';':
                tok.setType(TOK_SEMICOLON);
                return;
            case Digits09:
                _stream->putback((char)ch);
                scanInt(tok);
                return;
            case LowerCaseAz:
                _stream->putback((char)ch);
                scanId(tok);
                return;
            case '\r':
            case '\n':
                if (ch == '\r' && _stream->peek() == '\n')
                    _stream->get();
                ++_line;
                break;
            case ' ':
            case '\t':
                scanWhiteSpace();
                break;
            default:
                syntaxError("unknown character parsed ", ch, "'");
            }
        }
        tok.setType(TOK_EOF);
    }
};

class ConfigParser final : public ParserBase
{
private:
    using Api = std::map<String, int>;
    Api _data;

    void writeImpl(OStream& os, int format) override
    {
        // TODO: write the contents of _data to the stream
    }

public:
    ConfigParser()
    {
        _scanner = new ConfigScanner();
    }

    ~ConfigParser() override
    {
        delete _scanner;
    }

    void keyValueRule()
    {
        if (tokenType(0) != TOK_KEY)
            error("invalid token type");
        if (tokenType(1) != TOK_EQ)
            error("invalid token type");
        if (tokenType(2) != TOK_VALUE)
            error("invalid token type");
        if (tokenType(3) != TOK_SEMICOLON)
            error("invalid token type");

        // access the values stored at indices 0 and 2
        String key = _scanner->string(token(0).index());
        int value = _scanner->integer(token(2).index());

        if (_data.find(key) != _data.end())
            error("duplicate key");

        _data.insert(std::make_pair(key, value));

        // skip the 4 processed tokens
        advanceCursor(4);
    }

    void parseImpl(IStream& is) override
    {
        // make sure the token cursor is at zero
        // initially and attach the input stream
        // to the scanner
        _cursor = 0;
        _scanner->attach(&is);


        while (_cursor <= (int32_t)_tokens.size())
        {
            if (token(0).type() == TOK_EOF)
                break;

            const int32_t op = _cursor;
            keyValueRule();

            // if the cursor did not
            // advance force it to.
            if (op == _cursor)
                advanceCursor();
        }
    }

    bool hasKey(const String& v)
    {
        return _data.find(v) != _data.end();
    }

    int value(const String& v, const int defaultValue = -1)
    {
        if (const Api::iterator it = _data.find(v); it != _data.end())
            return it->second;
        return defaultValue;
    }
};
```
