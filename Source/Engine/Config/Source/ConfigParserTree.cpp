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

std::shared_ptr<blParserTree> blParserTree::addBranch(const std::string& name)
{
    auto node = std::make_shared<blParserTree>(this, name);
    _children[name] = node;
    return node;
}

std::shared_ptr<blParserTree> blParserTree::addLeaf(const std::string& name, const blParsedValue& value)
{
    auto node = std::make_shared<blParserTree>(this, name, value);
    _children[name] = node;
    return node;
}

blParserTree* blParserTree::getParent() const noexcept
{
    return _parent;
}

std::string& blParserTree::getName() const noexcept
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

std::map<std::string, std::shared_ptr<blParserTree>>& blParserTree::getChildren()
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

blParsedValue& blParserTree::getValue()
{
    return _value.value();
}


std::shared_ptr<blParserTree> blParserTree::find(const std::string& longName) noexcept
{
    std::string_view name = longName;

    return recursiveFind(name);
}

std::shared_ptr<blParserTree> blParserTree::recursiveFind(std::string_view& longName)
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

    return _children[key]->recursiveFind(next);
}

std::shared_ptr<blParserTree> blParserTree::insert(const std::string& longName, const blParsedValue& value) noexcept
{
    std::string_view view = longName;

    return recursiveInsert(view, value);
}

void blParserTree::foreach(const std::function<void(blParserTree&)>& func)
{
    recursiveForeach(func);
}

void blParserTree::recursiveForeach(const std::function<void(blParserTree&)>& func)
{    
    // Iterate through each child
    for (auto child : _children)
    {

        // Run the foreach function on the node 
        auto node = child.second;
        func(node);
        
        // If the node is a branch we must recurse down to it
        if (node->isBranch())
            node->recursiveForeach(func);
    }
}

std::shared_ptr<blParserTree> blParserTree::recursiveInsert(std::string_view& longName, const blParsedValue& value)
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

    // Add a branch if required
    if (!isLast && !_children.contains(key))
        return addBranch(key)->recursiveInsert(longName, value);

    // Add a leaf if required
    if (isLast && !_children.contains(key))


    if (isLast)
    {
        if (_children.contains(key))
            return _children[key]->_value = value;
        else 
            return addLeaf(key, value);
    } 
    else if (_children.contains(key))
    {
        return _children[key]->recursiveInsert(longName, value);
    }
    else 
    {
        return addBranch(key)->recursiveInsert(longName, value);
    }
}