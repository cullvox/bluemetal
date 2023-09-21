#include "Noodle/Noodle.h"
#include "Noodle/NoodleExceptions.h"
#include "NoodleLexer.h"

Noodle Noodle::parse(const std::string& noodle)
{
    NoodleLexer lexer(noodle);
    NoodleToken token;

    // Quick helper to go to the next token without any fuss.
    auto next = [&lexer, &token]() { token = lexer.next(); };

    // Get the first token from the lexer
    next();

    std::string identifier;
    std::string value;

    Noodle root("");
    Noodle* pCurrent = &root;

    // Parse through the file until the file has been completely lexed and parsed
    while (token.kind() != NoodleTokenKind::eEnd) {

        // The first token must be an identifier.
        if (!token.is(NoodleTokenKind::eIdentifier)) {
            throw NoodleParseException("expected identifier token");
        }

        identifier = std::string(token.lexeme());

        // After an identifier must come an equals
        next();
        if (!token.is(NoodleTokenKind::eEqual)) {
            throw NoodleParseException("expected equals \'=\' token");
        }

        // Must be a left curly, number or a string
        next();
        switch (token.kind()) {
        case NoodleTokenKind::eLeftCurly: {
            // Insert a new group noodle
            // Consecutive noodles will be inside of this group
            auto added = pCurrent->operator[](identifier) = Noodle(identifier, pCurrent)
            pCurrent = &added;
            next();
            continue;
        }

        case NoodleTokenKind::eInteger: {
            int i = (int)std::strtol(token.lexeme().data(), nullptr, 10);
            current->operator[](identifier) = Noodle(identifier, i, pCurrent);
            next();
            break;
        }

        case NoodleTokenKind::eFloat: {
            float f = std::strtof(token.lexeme().data(), nullptr);
            current->operator[](identifier) = Noodle(identifier, f, pCurrent);
            next();
            break;
        }

        case NoodleTokenKind::eBoolean: {
            bool b = (std::string(token.lexeme()) == "true") ? "true" : "false";
            current->operator[](identifier) = Noodle(identifier, b, pCurrent);
            next();
            break;
        }

        case NoodleTokenKind::eString: {
            current->operator[](identifier) = Noodle(identifier, std::string(token.lexeme()), pCurrent);
            next();
            break;
        }

        case NoodleTokenKind::eLeftBracket: {
            Noodle* array = &(current->operator[](identifier) = Noodle(identifier, current));
            array->_type = NoodleType::eArray;
            next(); // First element determines type of array.

            if (token.is(NoodleTokenKind::eRightBracket)) {
                break;
            }

            if (!token.isOneOf(NoodleTokenKind::eInteger, NoodleTokenKind::eFloat, NoodleTokenKind::eBoolean, NoodleTokenKind::eString)) {
                throw NoodleParseException("expected array type, integer, float, boolean, string or right bracket");
            }

            NoodleTokenKind expected = token.kind();

            while (true) {
                // Process the value
                if (token.kind() != expected) {
                    throw NoodleParseException("unexpected type change in array");
                }

                switch (expected) {
                case NoodleTokenKind::eInteger: {
                    int i = (int)std::strtol(token.lexeme().data(), nullptr, 10);
                    array->_array.emplace_back("", i, array);
                    break;
                }
                case NoodleTokenKind::eFloat: {
                    float f = std::strtof(token.lexeme().data(), nullptr);
                    array->_array.emplace_back("", f, array);
                    break;
                }
                case NoodleTokenKind::eBoolean: {
                    bool b = (std::string(token.lexeme()) == "true") ? "true" : "false";
                    array->_array.emplace_back("", b, current);
                    break;
                }
                case NoodleTokenKind::eString:
                    array->_array.emplace_back("", std::string(token.lexeme()), current);
                    break;
                default:
                    break; // These will be caught earlier
                }

                // Comma is expected or ]
                next();
                if (token.kind() == NoodleTokenKind::eComma) {
                    next();
                    continue;
                } else if (token.kind() == NoodleTokenKind::eRightBracket) {
                    next();
                    break;
                } else {
                    throw NoodleParseException("expected comma or right bracket in array");
                }
            }
            break;
        }

        default:
            throw NoodleParseException("expected left curly \'{\', integer, float or string token");
        }

        // Check if the next value is a comma, if it isn't we expect a right curly
        if (token.kind() == NoodleTokenKind::eComma) {
            next();

            if (token.kind() == NoodleTokenKind::eRightCurly) {
                goto parseCurlyEnds;
            }

            continue;
        }

    parseCurlyEnds:

        if (token.kind() == NoodleTokenKind::eRightCurly) {
            // Remove consecutive group endings after the last value
            do {
                if (current->parent()) {
                    current = current->parent();
                    next();
                } else {
                    throw NoodleParseException("unexpected right curly \'}\' token");
                }

            } while (token.kind() == NoodleTokenKind::eRightCurly);

            continue;
        } else {
            throw NoodleParseException("expected right curly \'}\' token");
        }
    }

    return root;
}

Noodle Noodle::parseFromFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::in);

    std::stringstream ss;
    ss << file.rdbuf();

    return parse(ss.str());
}

Noodle::Noodle(const std::string& groupName, Noodle* parent)
    : _type(NoodleType::eGroup)
    , _name(groupName)
    , _parent(parent)
    , _value()
{
}

Noodle::Noodle(const std::string& name, int value, Noodle* parent)
    : _type(NoodleType::eInteger)
    , _name(name)
    , _parent(parent)
    , _value(value)
{
}

Noodle::Noodle(const std::string& name, float value, Noodle* parent)
    : _type(NoodleType::eFloat)
    , _name(name)
    , _parent(parent)
    , _value(value)
{
}

Noodle::Noodle(const std::string& name, bool value, Noodle* parent)
    : _type(NoodleType::eBoolean)
    , _name(name)
    , _parent(parent)
    , _value(value)
{
}

Noodle::Noodle(const std::string& name, const std::string& value, Noodle* parent)
    : _type(NoodleType::eString)
    , _name(name)
    , _parent(parent)
    , _value(value)
{
}

NoodleType Noodle::arrayType()
{
    if (_type != NoodleType::eArray) {
        throw NoodleInvalidAccessException("type was not an array");
    }

    return _arrayType;
}

size_t Noodle::size()
{
    switch (_type) {
    case NoodleType::eGroup:
        return _groupChildren.size();
    case NoodleType::eArray:
        return _array.size();
    default:
        return 0;
    }
}

std::string Noodle::dump()
{
    std::stringstream ss;
    recursiveDump(ss, 0);
    return ss.str();
}

void Noodle::dumpToFile(const std::filesystem::path& path)
{
    std::ofstream file(path);

    std::stringstream ss;
    recursiveDump(ss, 0);
    file << ss.str();
}

std::string Noodle::toString() const
{
    switch (_type) {
    case NoodleType::eGroup:
        return _name;
    case NoodleType::eInteger:
        return std::to_string(std::get<int>(_value));
    case NoodleType::eFloat:
        return std::to_string(std::get<float>(_value));
    case NoodleType::eBoolean:
        return std::string(std::get<bool>(_value) ? "true" : "false");
    case NoodleType::eString:
        return "\"" + std::get<std::string>(_value) + "\"";
    case NoodleType::eArray: {
        std::stringstream ss;
        ss << "[ ";
        bool isFirst = true;
        for (const Noodle& noodle : _array) {
            if (!isFirst)
                ss << ", ";
            else
                isFirst = false;

            ss << noodle;
        }
        ss << " ]";
        return ss.str();
    }
    default:
        return std::string();
    }
}


Noodle& Noodle::operator[](const std::string& key)
{
    if (_type != NoodleType::eGroup) {
        throw NoodleInvalidAccessException("cannot access a group from a non group noodle");
    }

    auto& noodle = _groupChildren[key];
    noodle._parent = this;

    return noodle;
}

Noodle& Noodle::operator[](size_t index)
{
    if (_type != NoodleType::eArray) {
        throw NoodleInvalidAccessException("cannot access an array from a non array noodle");
    }

    return _array[index];
}

std::ostream& operator<<(std::ostream& os, const Noodle& noodle)
{
    os << noodle.toString();
    return os;
}

void Noodle::recursiveDump(std::stringstream& ss, int tabs) const
{
    auto offset = [&]() {
        for (int i = 0; i < tabs; i++) {
            ss << '\t';
        }
    };

    offset();

    if (_type == NoodleType::eGroup) {
        // Don't output a name or } on the root node
        if (!isRoot()) {
            tabs++;
            ss << _name << " = {\n";
        }

        for (const auto& pair : _groupChildren) {
            const Noodle& noodle = pair.second;

            noodle.recursiveDump(ss, tabs);
        }

        if (!isRoot())
            tabs--;

        offset();

        if (!isRoot())
            ss << "}\n";
        return;
    }

    // Add name and equals sign
    ss << _name << " = " << toString() << ",\n";
}
