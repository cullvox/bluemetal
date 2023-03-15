#include <iostream>
#include <optional>

#include "Engine/Engine.hpp"

int main(int argc, const char** argv)
{
    bl::Engine engine("Dark Red");
    
    engine.run();

    return 0;
}