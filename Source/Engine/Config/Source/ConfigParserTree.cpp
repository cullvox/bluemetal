#include "Config/ConfigParserTree.hpp"

blParserTree::blParserTree(const blParserTree& other)
    : _parent(nullptr)
    , _children()
    , _name(other._name)
    , _value()
{
    // Make a deep copy of the other tree
    other.foreach([&](const blParserTree& node){
        if (node.isLeaf())
            insert(node.getLongName(), node.getValue());
    });
}

blParserTree::blParserTree(blParserTree&& other)
    : _parent(std::move(other._parent))
    , _children(std::move(other._children))
    , _name(std::move(other._name))
    , _value(std::move(other._value))
{
}

blParserTree& blParserTree::operator=(const blParserTree& rhs)
{
    // Make a deep copy of the other tree
    rhs.foreach([&](const blParserTree& node){
        if (node.isLeaf())
            insert(node.getLongName(), node.getValue());
    });
    return *this;
}

blParserTree& blParserTree::operator=(blParserTree&& rhs)
{
    _parent = std::move(rhs._parent);
    _children = std::move(rhs._children);
    _name = std::move(rhs._name);
    _value = std::move(rhs._value);
    return *this;
}

blParserTree& blParserTree::addBranch(const std::string& name)
{
    auto& node = _children[name]; 
    node._parent = this;
    node._name = name;
    return node;
}

blParserTree& blParserTree::addLeaf(const std::string& name, blParsedValue value)
{
    auto& node = _children[name];
    node._parent = this;
    node._name = name;
    node._value = value;
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
    std::string longName = getName();
    blParserTree* current = getParent();

    // Traverse back up the tree for parents and their names    
    while(current != nullptr && not current->getName().empty())
    {
        longName = current->getName() + "." + longName;
        current = current->getParent();
    }
    return longName;
}

const std::map<std::string, blParserTree>& blParserTree::getChildren() const noexcept
{
    return _children;
}

bool blParserTree::isLeaf() const noexcept
{
    return _value.has_value();
}

bool blParserTree::isBranch() const noexcept
{
    return !_value.has_value();
}

const blParsedValue& blParserTree::getValue() const noexcept
{
    return _value.value();
}

blParsedValue& blParserTree::getValue() noexcept
{
    return _value.value();
}

void blParserTree::setName(const std::string& name) noexcept
{
    _name = name;
}

void blParserTree::setValue(blParsedValue value) noexcept
{
    _value = value;
}

blParserTree* blParserTree::find(const std::string& longName) noexcept
{
    std::string_view name = longName;

    return recursiveFind(name);
}

blParserTree* blParserTree::recursiveFind(std::string_view& longName) noexcept
{

    // Find the name before the next '.'
    auto dotpos = longName.find_first_of('.', 0);
    auto current = longName.substr(0, dotpos); 
    auto key = std::string(current);

    if (dotpos == std::string::npos)
    {
        key = std::string(longName);
    }

    bool isLast = (dotpos == std::string::npos); 

    // The path ends here, so return nullptr
    if (_children.contains(key) && isLast)
    {
        return &_children.at(key);
    } 
    else if (_children.contains(key))
    {
        // Find the next key in this child
        auto next = std::string_view(longName.begin() + dotpos + 1, longName.end());

        return _children[key].recursiveFind(next);
    } 
    else
    {
        return nullptr;
    }
}

blParserTree& blParserTree::insert(const std::string& longName, blParsedValue value) noexcept
{
    std::string_view view = longName;

    return recursiveInsert(view, value);
}

void blParserTree::foreach(const std::function<void(blParserTree&)>& func) noexcept
{
    recursiveForeach(func);
}

void blParserTree::foreach(const std::function<void(const blParserTree&)>& func) const noexcept
{
    recursiveForeach(func);
}

void blParserTree::recursiveForeach(const std::function<void(blParserTree&)>& func) noexcept
{    
    // Iterate through each child
    for (auto& child : _children)
    {

        // Run the foreach function on the node 
        auto& node = child.second;
        func(node);
        
        // If the node is a branch we must recurse down to it
        if (node.isBranch())
            node.recursiveForeach(func);
    }
}

void blParserTree::recursiveForeach(const std::function<void(const blParserTree&)>& func) const noexcept
{    
    // Iterate through each child
    for (auto& child : _children)
    {

        // Run the foreach function on the node 
        auto& node = child.second;
        func(node);
        
        // If the node is a branch we must recurse down to it
        if (node.isBranch())
            node.recursiveForeach(func);
    }
}

blParserTree& blParserTree::recursiveInsert(std::string_view& longName, const blParsedValue& value) noexcept
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
    else
    {
        // Find the next key in this child
        longName = std::string_view(longName.begin() + dotpos + 1, longName.end());
    }

    // Set the value
    if (isLast && _children.contains(key))
    {
        auto& node = _children[key];
        node._value = value;
        return node;
    }
    // Use the branch to insert
    else if (!isLast && _children.contains(key))
    {
        return _children[key].recursiveInsert(longName, value);
    }
    // Add a branch
    else if (!isLast && !_children.contains(key))
    {
        return addBranch(key).recursiveInsert(longName, value);
    }
    // Add a leaf 
    else
    {
        return addLeaf(key, value);
    }
}