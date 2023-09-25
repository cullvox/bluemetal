#pragma once

#include "NoodleToken.h"
#include "Export.h"

namespace bl {

namespace NoodleLexerUtils {
    static BLUEMETAL_API bool isSpace(char c);
    static BLUEMETAL_API bool isDigit(char c);
    static BLUEMETAL_API bool isIdentifierChar(char c);
} // namespace NoodleLexerUtils

/** @brief Lexes noodle strings into tokens for a parser. */
class BLUEMETAL_API NoodleLexer {
public:

    /** @brief Constructor */
    NoodleLexer(const std::string& content) noexcept;

    /** @brief Returns the next token and increases the line and column positions. */
    [[nodiscard]] NoodleToken next() noexcept;

    /** @brief Returns the current row of the lexers position. */
    [[nodiscard]] int getRow() const noexcept;

    /** @brief Returns the current column of the lexers position. */
    [[nodiscard]] int getCol() const noexcept;
    
private:

    /** @brief Peeks the next character without incrementing the iterator. */
    [[nodiscard]] char peek() const noexcept;

    /** @brief Gets the next character and increments iterator. */
    char get() noexcept;

    /** @brief Creates a basic identifier token. */
    [[nodiscard]] NoodleToken createIdentifier() noexcept;

    /** @brief Creates a basic string token. */
    [[nodiscard]] NoodleToken createString() noexcept;

    /** @brief Creates a basic number token. */
    [[nodiscard]] NoodleToken createNumber() noexcept;

    /** @brief Creates a basic token of any kind. */
    [[nodiscard]] NoodleToken createAtom(NoodleTokenKind kind) noexcept;

    const std::string&          _content;    /** @brief Original content of string provided to lexer. */
    std::string::const_iterator _it;         /** @brief Iterator of @ref _content. */
    int                         _row;        /** @brief Current line number. */
    int                         _col;        /** @brief Current character position. */
};

} // namespace bl
