#pragma once

#include "Core/Precompiled.hpp"

using blParsedValue = std::variant<int, std::string_view>;

class blParserTree
{
public:
    blParserTree() = default;
    blParserTree(blParserTree& parent, const std::string_view& name);
    blParserTree(blParserTree& parent, const std::string_view& name, const blParsedValue& value);
    blParserTree(const std::unordered_map<std::string, blParsedValue>& map);
    ~blParserTree() = default;

    blParserTree& addChild(const std::string_view& name);
    blParserTree& addChild(const std::string& name);
    blParserTree& addChild(const std::string_view& name, const blParsedValue& value);
    blParserTree& addChild(const std::string& name, const blParsedValue& value);
    blParserTree* getParent() const noexcept;
    const std::string& getName() const noexcept;
    std::string getLongName() const noexcept;
    std::map<std::string, blParserTree> getChildren();
    void recurseBuildValues(std::unordered_map<std::string, blParsedValue>& list) const;
    bool isLeaf() const noexcept;
    const blParsedValue& getValue() const;
    blParsedValue& getValue();
    blParserTree* recurseFind(const std::string& longName);
    blParserTree* insert(const std::string& longName, const blParsedValue& value);

private:
    blParserTree* recurseFindImpl(std::string_view& longName);
    blParserTree* recurseInsert(std::string_view& longName, const blParsedValue& value);

    blParserTree*                       _parent;
    std::map<std::string, blParserTree> _children;
    std::string                         _name;
    std::optional<blParsedValue>        _value;
};