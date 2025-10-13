#pragma once

class Node
{
public:
    Node() = default;
    virtual ~Node() = default;

    virtual void Update(float deltaTime) {}
    virtual void PhysicsUpdate(float delta) {}
    virtual void Draw() {}
};
