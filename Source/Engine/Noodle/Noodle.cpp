#include "Noodle.h"
#include "NoodleLexer.h"
#include "Core/Print.h"

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
            blError("Noodle Parser ({}:{}): Expected identifier token not {}.", lexer.getRow(), lexer.getCol(), token.getKind());
        }

        identifier = std::string(token.lexeme());

        // After an identifier must come an equals
        next();
        if (!token.is(NoodleTokenKind::eEqual)) {
            blError("Noodle Parser ({}:{}): Expected equals \'=\' token after identifier, got {}.", lexer.getRow(), lexer.getCol(), token.getKind());
            break;
        }

        // Must be a left curly, integer, float, boolean, or a string
        next();
        switch (token.getKind()) {
        case NoodleTokenKind::eLeftCurly: {
            // Insert a new group noodle
            // Consecutive noodles will be inside of this group
            auto added = pCurrent->operator[](identifier) = Noodle(identifier, pCurrent)
            pCurrent = &added;
            next();
            continue;
        }

        case NoodleTokenKind::eInteger: {
            int i = (int)std::strtol(token.getLexeme().data(), nullptr, 10);
            current->operator[](identifier) = Noodle(identifier, i, pCurrent);
            next();
            break;
        }

        case NoodleTokenKind::eFloat: {
            float f = std::strtof(token.getLexeme().data(), nullptr);
            current->operator[](identifier) = Noodle(identifier, f, pCurrent);
            next();
            break;
        }

        case NoodleTokenKind::eBoolean: {
            bool b = token.getLexeme() == "true" ? true : false; // Lexer asserts that it must be true or false string.
            current->operator[](identifier) = Noodle(identifier, b, pCurrent);
            next();
            break;
        }

        case NoodleTokenKind::eString: {
            current->operator[](identifier) = Noodle(identifier, std::string(token.getLexeme()), pCurrent);
            next();
            break;
        }

        case NoodleTokenKind::eLeftBracket: {
            Noodle* pArray = &(current->operator[](identifier) = Noodle(identifier, current));
            processArray(pArray, lexer);
            next();
            break;
        }

        default:
            blError("Noodle Parser ({}:{}): Expected group (\'{\') or value (int, float, bool...) token, got {}.", lexer.getRow(), lexer.getCol(), token.getKind());
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
                    blError("Noodle Parser ({}:{}): Unexpected right curly \'}\' token, got {}.", lexer.getRow(), lexer.getCol(), token.getKind());
                }

            } while (token.kind() == NoodleTokenKind::eRightCurly);

            continue;
        } else {
            blError("Noodle Parser ({}:{}): Expected right curly \'}\' token, got {}.", lexer.getRow(), lexer.getCol(), token.getKind());
        }
    }

    return root;
}

Noodle Noodle::parseFromFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::in);

    std::stringstream ss;
    ss << file.rdbuf();

    blInfo("Noodle parsing file: {}", path);
    return parse(ss.str());
}

Noodle::Noodle(const std::string& groupName, Noodle* parent)
    : _type(NoodleType::eGroup), _name(groupName), _parent(parent), _value()
{
}

Noodle::Noodle(const std::string& name, int value, Noodle* parent)
    : _type(NoodleType::eInteger), _name(name), _parent(parent), _value(value)
{
}

Noodle::Noodle(const std::string& name, float value, Noodle* parent)
    : _type(NoodleType::eFloat), _name(name), _parent(parent), _value(value)
{
}

Noodle::Noodle(const std::string& name, bool value, Noodle* parent)
    : _type(NoodleType::eBoolean), _name(name), _parent(parent), _value(value)
{
}

Noodle::Noodle(const std::string& name, const std::string& value, Noodle* parent)
    : _type(NoodleType::eString), _name(name), _parent(parent), _value(value)
{
}

Noodle* Noodle::getParent() const noexcept { return _parent; }

NoodleType Noodle::getType() const noexcept { return _type; }

size_t Noodle::getSize() const noexcept
{
    switch (_type) {
    case NoodleType::eGroup: return _groupChildren.size();
    case NoodleType::eArray: return _array.size();
    default:
        return 0;
    }
}

bool Noodle::setName(const std::string& name) noexcept
{
    // Change the name from the parents map
    if (_parent) {
        auto& children = _parent._groupChildren;
        
        if (children.contains(name)) {
            blError("Cannot change name, already exists!");
            return false;
        }
        
        children.erase(_name);
        children.insert({name, *this}); 
    }

    _name = name;
}

bool Noodle::isInteger() const noexcept
{
    return _type == NoodleType::eInteger;
}

bool Noodle::isFloat() const noexcept
{
    return _type == NoodleType::eFloat;
}

bool Noodle::isBool() const noexcept
{
    return _type == NoodleType::eBoolean;
}

bool Noodle::isString() const noexcept
{
    return _type == NoodleType::eString;
}

bool Noodle::isGroup() const noexcept
{
    return _type == NoodleType::eGroup;
}

bool Noodle::isArray() const noexcept
{
    return _type == NoodleType::eArray;
}

bool Noodle::get(int& value) const noexcept
{
    if (_type != NoodleType::eInteger) {
        blError("Type not an integer!");
        return false;
    }

    value = std::get<int>(_value);
    return true;
}

bool Noodle::get(float& value) const noexcept
{
    if (_type != NoodleType::eFloat) {
        blError("Type not a float!");
        return false;
    }

    value = std::get<float>(_value);
    return true;
}

bool Noodle::get(bool& value) const noexcept
{
    if (_type != NoodleType::eBoolean) {
        blError("Type not a bool!");
        return false;
    }

    value = std::get<bool>(_value);
    return true;
}

bool Noodle::get(std::string& value) const noexcept
{
    if (_type != NoodleType::eString) {
        blError("Type not a string!");
        return false;
    }

    value = std::get<std::string>(_value);
    return true;
}

bool Noodle::isRoot() const noexcept 
{ 
    return _parent == nullptr; 
}

bool contains(const std::string& key) const noexcept
{
    if (_type != NoodleType::eGroup) {
        blWarning("Type is not a group!");
        return false;
    }
    
    return _groupChildren.contains(key);
}

std::string Noodle::dump() const noexcept
{
    std::stringstream ss;
    recursiveDump(ss, 0);
    return ss.str();
}

bool Noodle::dumpToFile(const std::filesystem::path& path) const noexcept
{
    std::ofstream file(path);
    if (file.bad()) {
        blError("Could not open dump file: {}", path); 
        return false;
    }

    std::stringstream ss;
    recursiveDump(ss, 0);
    file << ss.str();
    return true;
}

std::string Noodle::toString() const noexcept
{
    switch (_type) {
    case NoodleType::eGroup:   return _name;
    case NoodleType::eInteger: return std::to_string(std::get<int>(_value));
    case NoodleType::eFloat:   return std::to_string(std::get<float>(_value));
    case NoodleType::eBoolean: return std::string(std::get<bool>(_value) ? "true" : "false");
    case NoodleType::eString:  return fmt::format_to("\"{}\"", std::get<std::string(_value));
    case NoodleType::eArray: {
        switch(_array.index()) {
        case 0: return fmt::format_to("[ {} ]", _array.get<std::vector<int>>());
        case 1: return fmt::format_to("[ {} ]", _array.get<std::vector<float>>());
        case 2: return fmt::format_to("[ {} ]", _array.get<std::vector<bool>>());
        case 3: return fmt::format_to("[ {} ]", _array.get<std::vector<std::string>>());
        default: assert(false && "std::variant failed, this should never happen.");
        }
    }
    default:
        return {};
    }
}

int Noodle::getInt() const
{   
    if (_type != NoodleType::eInteger)
        throw std::runtime_error("Type must be integer if you are reading it!");
    return std::get<int>(_value);
}

float Noodle::getFloat() const
{   
    if (_type != NoodleType::eFloat)
        throw std::runtime_error("Type must be float if you are reading it!");
    return std::get<float>(_value);
}

bool Noodle::getBool() const
{   
    if (_type != NoodleType::eBoolean)
        throw std::runtime_error("Type must be bool if you are reading it!");
    return std::get<float>(_value);
}

std::string Noodle::getString() const
{   
    if (_type != NoodleType::eString)
        throw std::runtime_error("Type must be string if you are reading it!");
    return std::get<float>(_value);
}

NoodleType Noodle::arrayType()
{
    if (_type != NoodleType::eArray)
        throw std::runtime_error("Type must be array if you are reading it!");
    return _arrayType;
}

Noodle& Noodle::operator[](const std::string& key)
{
    if (_type != NoodleType::eGroup) {
        throw std::runtime_error("Cannot access a key from a non group noodle!");
    }

    auto& noodle = _groupChildren[key];
    noodle._parent = this;

    return noodle;
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

bool processArray(Noodle* pArray) {
    
    array->_type = NoodleType::eArray;

    // First element determines type of array.
    next(); 

    if (token.is(NoodleTokenKind::eRightBracket)) {
        break;
    }

    if (!token.isOneOf(NoodleTokenKind::eInteger, NoodleTokenKind::eFloat, NoodleTokenKind::eBoolean, NoodleTokenKind::eString)) {
        blError("Noodle Parser ({}:{}): Expected array type, integer, float, boolean, string or right bracket.", lexer.getRow(), lexer.getCol());
        break;
    }

    NoodleTokenKind expected = token.getKind();

    while (true) {
        // Process the value
        if (!token.is(expected)) {
            blError("Noodle Parser ({}:{}): Unexpected type change in array, expected \'{}\' got \'{}\'., ", lexer.getRow(), lexer.getCol(), expected, token.getKind());
            break;
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
