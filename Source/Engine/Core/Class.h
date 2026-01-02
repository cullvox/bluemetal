#pragma once

#include <string>
#include <vector>

class Property {
    std::string name;
    std::string type;
    bool isEditable;
};

class Class {

public:
    Class() = default;
    virtual ~Class() = default;
    virtual Class* Clone() = 0;

    void SetProperty(const std::string& name, std::variant value);

    std::vector<Property> GetProperties();

}