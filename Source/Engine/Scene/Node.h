#pragma once

class Node
{
public:
    Node() = default;
    virtual ~Node() = default;

    virtual void Update(float deltaTime) { (void)deltaTime; }
    virtual void PhysicsUpdate(float delta) { (void)delta; }
    virtual void Draw() {}
};
