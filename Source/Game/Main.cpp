#include "Engine/Engine.hpp"

int main(int argc, const char** argv)
{
    (void)argc;
    (void)argv;

    blEngine engine("Dark Red");
    
    engine.run();

    return 0;
}