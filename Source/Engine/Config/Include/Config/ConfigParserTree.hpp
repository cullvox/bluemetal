#pragma once

#include "Core/Precompiled.hpp"

using blParsedValue = std::variant<int, std::string>;

class blParserTree
{
public:
    blParserTree() = default;
    blParserTree(const blParserTree& other);
    blParserTree(blParserTree&& other);
    ~blParserTree() = default;

    blParserTree& operator=(const blParserTree& rhs);
    blParserTree& operator=(blParserTree&& rhs);

    blParserTree& addBranch(const std::string& name);
    blParserTree& addLeaf(const std::string& name, blParsedValue value);

    blParserTree* getParent() const noexcept;
    const std::map<std::string, blParserTree>& getChildren() const noexcept;
    const std::string& getName() const noexcept;
    const blParsedValue& getValue() const noexcept;
    blParsedValue& getValue() noexcept;
    void setName(const std::string& name) noexcept;
    void setValue(blParsedValue value) noexcept;
    bool isLeaf() const noexcept;
    bool isBranch() const noexcept;
    void foreach(const std::function<void(blParserTree&)>& func) noexcept;
    void foreach(const std::function<void(const blParserTree&)>& func) const noexcept;

    // long name format "window.width", "render.physicalDevice.name" 
    std::string getLongName() const noexcept;
    blParserTree* find(const std::string& longName) noexcept;
    blParserTree& insert(const std::string& longName, blParsedValue value) noexcept;

private:
    blParserTree* recursiveFind(std::string_view& longName) noexcept;
    blParserTree& recursiveInsert(std::string_view& longName, const blParsedValue& value) noexcept;
    void recursiveForeach(const std::function<void(blParserTree&)>& func) noexcept;
    void recursiveForeach(const std::function<void(const blParserTree&)>& func) const noexcept;

    blParserTree*                           _parent;
    std::map<std::string, blParserTree>     _children;
    std::string                             _name;
    std::optional<blParsedValue>            _value;
};