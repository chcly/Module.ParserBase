/*
-------------------------------------------------------------------------------
    Copyright (c) Charles Carley.

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#include <cstdio>
#include "ParserBase/ParserBase.h"
#include "TestDirectory.h"
#include "Utils/Char.h"
#include "gtest/gtest.h"

using namespace Rt2;

enum TokenId
{
    TOK_ERROR = -2,
    TOK_EOF,
    TOK_NULL,
    TOK_KEY,
    TOK_VALUE,
    TOK_EQ        = '=',
    TOK_SEMICOLON = ';'
};



class ConfigScanner final : public ScannerBase
{
public:
    void scanInt(TokenBase& tok)
    {
        String dest;

        int ch = _stream->get();
        while (isDecimal(ch))
        {
            dest.push_back((char)ch);
            ch = _stream->get();
            if (ch <= 0)
                syntaxError("unexpected end of file");
        }
        _stream->putback((char)ch);

        tok.setIndex(save(Char::toInt32(dest)));
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
        tok.setIndex(save(dest));
        tok.setType(TOK_KEY);
    }


    void scan(TokenBase& tok) override
    {
        if (_stream == nullptr)
            syntaxError("No supplied stream");

        tok.setType(TOK_NULL);
        tok.clear();

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
                syntaxError("unknown character parsed #x", Char::toHexString((uint8_t)ch), "'");
            }
        }

        tok.setType(TOK_EOF);
    }
};

using Api = std::map<String, int>;

class ConfigParser final : public ParserBase
{
private:
    Api _data;

public:
    ConfigParser()
    {
        _scanner = new ConfigScanner();
    }


    ~ConfigParser() override
    {
        delete _scanner;
    }


    void writeImpl(OStream& is, int format) override
    {
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

        String key   = _scanner->string(token(0).index());
        int    value = _scanner->integer(token(2).index());

        if (_data.find(key) != _data.end())
            error("duplicate key");
        _data.insert(std::make_pair(key, value));
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
        if (const Api::const_iterator it = _data.find(v); it != _data.end())
            return it->second;
        return defaultValue;
    }
};


GTEST_TEST(ParserBase, Parse_001)
{
    ConfigParser parser;

    StringStream ss;
    ss << "# line comment" << std::endl;
    ss << "a=1;";
    ss << "b=2;";
    parser.read(ss);


    EXPECT_TRUE(parser.hasKey("a"));
    EXPECT_TRUE(parser.hasKey("b"));
    EXPECT_EQ(parser.value("a"), 1);
    EXPECT_EQ(parser.value("b"), 2);
}

GTEST_TEST(ParserBase, Scan_001)
{
    ConfigScanner scanner;

    StringStream ss;
    ss << "# line comment" << std::endl;
    ss << "a=1;";
    ss << "b=2;";
    scanner.attach(&ss, PathUtil());

    TokenBase t0;

    scanner.scan(t0);
    EXPECT_EQ(t0.type(), TOK_KEY);
    EXPECT_EQ(scanner.string(t0.index()), "a");


    scanner.scan(t0);
    EXPECT_EQ(t0.type(), TOK_EQ);

    scanner.scan(t0);
    EXPECT_EQ(t0.type(), TOK_VALUE);
    EXPECT_EQ(scanner.integer(t0.index()), 1);

    scanner.scan(t0);
    EXPECT_EQ(t0.type(), TOK_SEMICOLON);


    scanner.scan(t0);
    EXPECT_EQ(t0.type(), TOK_KEY);
    EXPECT_EQ(scanner.string(t0.index()), "b");


    scanner.scan(t0);
    EXPECT_EQ(t0.type(), TOK_EQ);

    scanner.scan(t0);
    EXPECT_EQ(t0.type(), TOK_VALUE);
    EXPECT_EQ(scanner.integer(t0.index()), 2);

    scanner.scan(t0);
    EXPECT_EQ(t0.type(), TOK_SEMICOLON);

    scanner.scan(t0);
    EXPECT_EQ(t0.type(), TOK_EOF);
}