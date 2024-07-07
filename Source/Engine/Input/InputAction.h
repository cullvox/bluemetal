#pragma once

#include "Core/De"

namespace bl {

enum class InputKey {
    A,
    B,
    C,
    D,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    
};

struct InputAction {
    std::string name;

};

struct InputAxis {
    std::string name;

};

}