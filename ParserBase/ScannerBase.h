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
#pragma once
#include "ParserBase/TokenBase.h"
#include "Utils/FileSystem.h"
#include "Utils/IndexCache.h"
#include "Utils/Path.h"

namespace Rt2
{
    using StringTable = IndexCache<String>;
    using IntTable    = IndexCache<int>;

    constexpr char MultiLineCommentStop0 = '-';
    constexpr char MultiLineCommentStop1 = '>';

    class ScannerBase
    {
    protected:
        IStream*    _stream;
        StringTable _stringTable;
        IntTable    _intTable;
        PathUtil    _file;
        size_t      _line;

        size_t save(const String& str);

        size_t save(const int& val);

        virtual void cleanup();

        template <typename... Args>
        [[noreturn]] void syntaxError(
            const String& what, Args&&... args);

        void scanWhiteSpace() const;

        void scanLineComment();

        void scanMultiLineComment();

        void scanAny(String& dest, char seqStart, char seqEnd);

        void extractCode(String& dest, char seqStart, char seqEnd);

    private:
        [[noreturn]] void syntaxErrorThrow(const String& message) const;

    public:
        ScannerBase();

        virtual ~ScannerBase() = default;

        virtual void scan(TokenBase& tok) = 0;

        void attach(IStream* stream, const PathUtil& file);

        void attach(IStream* stream);

        const String& string(const size_t& i) const;

        void string(String& dest, const size_t& i) const;

        int integer(const size_t& i) const;

        size_t line() const;

        bool containsString(size_t id) const;
    };

    template <typename... Args>
    void ScannerBase::syntaxError(const String& what, Args&&... args)
    {
        OutputStringStream oss;
        oss << what;
        ((oss << std::forward<Args>(args)), ...);
        syntaxErrorThrow(oss.str());
    }

    inline size_t ScannerBase::line() const
    {
        return _line;
    }

}  // namespace Rt2
