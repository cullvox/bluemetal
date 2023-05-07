#include "Config/ConfigParserTree.hpp"

blParserTree::blParserTree(blParserTree& parent, const std::string_view& name)
    : _parent(&parent)
    , _name(std::string(name))
    , _value(std::nullopt)
{
}

blParserTree::blParserTree(blParserTree& parent, const std::string_view& name, const blParsedValue& value)
    : _parent(&parent)
    , _name(std::string(name))
    , _value(value)
{
}

blParserTree::blParserTree(const std::unordered_map<std::string, blParsedValue>& map)
    : _parent()
    , _name()
    , _value(std::nullopt)
{

    for (const auto& pair : map)
    {
        insert(pair.first, pair.second);
    }
}

blParserTree& blParserTree::addChild(const std::string_view& name)
{
    auto& node = _children[name.data()]; 
    node._name = name;
    node._parent = this;

    return node;
}

blParserTree& blParserTree::addChild(const std::string& name)
{
    auto& node = _children[name]; 
    node._name = name;
    node._parent = this;

    return node;
}

blParserTree& blParserTree::addChild(const std::string_view& name, const blParsedValue& value)
{
    auto& node = _children[name.data()]; 
    node._name = name;
    node._value = value;
    node._parent = this;

    return node;
}

blParserTree& blParserTree::addChild(const std::string& name, const blParsedValue& value)
{
    auto& node = _children[name]; 
    node._name = name;
    node._value = value;
    node._parent = this;

    return node;
}

blParserTree* blParserTree::getParent() const noexcept
{
    return _parent;
}

const std::string& blParserTree::getName() const noexcept
{
    return _name;
}

std::string blParserTree::getLongName() const noexcept
{
    std::stringstream ss;
    blParserTree* current = _parent;
    while(current != nullptr)
    {
        ss << current->_name << '.';
        current = current->_parent;
    }
    return ss.str();
}

void blParserTree::recurseBuildValues(std::unordered_map<std::string, blParsedValue>& list) const
{
    for (const auto& child : _children)
    {
        const blParserTree& node = child.second;
       
        if (node.isLeaf())
        {
            list[node.getLongName()] = node.getValue();
        }
        else
        {
            node.recurseBuildValues(list);
        }
    }
}

bool blParserTree::isLeaf() const noexcept
{
    return _value.has_value();
}

const blParsedValue& blParserTree::getValue() const
{
    return _value.value();
}

blParsedValue& blParserTree::getValue()
{
    return _value.value();
}

blParserTree* blParserTree::recurseFind(const std::string& longName)
{
    std::string_view name = longName;
    return recurseFindImpl(name);
}

blParserTree* blParserTree::recurseFindImpl(std::string_view& longName)
{

    // Find the name before the next '.'
    auto dotpos = longName.find_first_of('.', 0);
    auto current = longName.substr(0, dotpos); 
    auto key = std::string(current);

    if (dotpos == std::string::npos)
    {
        key = std::string(longName);
    }

    // The path ends here, so return nullptr
    if (not _children.contains(key)) return nullptr;

    // Find the next key in this child
    auto next = std::string_view(longName.begin() + dotpos + 1, longName.end());

    return _children[key].recurseFindImpl(next);
}

blParserTree* blParserTree::insert(const std::string& longName, const blParsedValue& value)
{
    std::string_view view = longName;
    return recurseInsert(view, value);
}

blParserTree* blParserTree::recurseInsert(std::string_view& longName, const blParsedValue& value)
{
    // Find the name before the next '.'
    auto dotpos = longName.find_first_of('.', 0);
    auto current = longName.substr(0, dotpos); 
    auto key = std::string(current);

    bool isLast = false;

    if (dotpos == std::string::npos)
    {
        key = std::string(longName);
        isLast = true;
    }

    // Find the next key in this child
    longName = std::string_view(longName.begin() + dotpos + 1, longName.end());

    // Check if any of the children already have this value
    if (isLast)
    {
        return &addChild(key, value);
    } 
    else if (_children.contains(key))
    {
        return recurseInsert(longName, value);
    }
    else 
    {
        return addChild(key).recurseInsert(longName, value);
    }
}