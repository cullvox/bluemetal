#include "NoodleToken.h"

namespace bl {

NoodleToken::NoodleToken(NoodleTokenKind kind, const std::string_view lexeme, int row, int col)
    : _kind(kind)
    , _row(row)
    , _col(col)
    , _lexeme(lexeme) {}

}
