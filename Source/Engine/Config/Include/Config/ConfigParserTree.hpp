#pragma once

#include "Core/Precompiled.hpp"

using blParsedValue = std::variant<int, std::string>;

class blParserTree
{
public:
    using blParserTreeForeachFunc = std::function<void(std::shared_ptr<blParserTree>)>;

    blParserTree() = default;
    blParserTree(blParserTree* parent, const std::string& name);
    blParserTree(blParserTree* parent, const std::string& name, const blParsedValue& value);
    ~blParserTree() = default;

    blParserTree* getParent() const noexcept;
    std::map<std::string, std::shared_ptr<blParserTree>>& getChildren();
    std::shared_ptr<blParserTree> addBranch(const std::string& name);
    std::shared_ptr<blParserTree> addLeaf(const std::string& name, const blParsedValue& value);
    bool isLeaf() const noexcept;
    bool isBranch() const noexcept;
    std::string& getName() noexcept;
    blParsedValue& getValue() noexcept;
    

    // long name format "window.width", "render.physicalDevice.name" 
    std::string getLongName() const noexcept;
    std::shared_ptr<blParserTree> find(const std::string& longName) noexcept;
    std::shared_ptr<blParserTree> insert(const std::string& longName, const blParsedValue& value) noexcept;

    void foreach(const std::function<void(blParserTree&)>& func);

private:
    std::shared_ptr<blParserTree> recursiveFind(std::string_view& longName);
    std::shared_ptr<blParserTree> recursiveInsert(std::string_view& longName, const blParsedValue& value);
    void recursiveForeach(const std::function<void(blParserTree&)>& func);

    blParserTree*                                           _parent;
    std::map<std::string, std::shared_ptr<blParserTree>>    _children;
    std::string                                             _name;
    std::optional<blParsedValue>                            _value;
};