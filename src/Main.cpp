#include <iostream>

#include "config/Config.hpp"

int main(int argc, const char** argv)
{
    std::cout << "My Test" << std::endl;
    CConfig newConfig{};
    newConfig.Save();
}