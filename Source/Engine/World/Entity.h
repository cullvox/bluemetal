#pragma once

#include "Precompiled.h"

namespace bl
{

struct RenderData
{
    uint32_t currentFrame;
};

class SerializeData;
class DeserializeData;

/** @brief An entity is an object that lives in the game world.
 * 
 * It doesn't need to physically exist in the scene or produce anything.
 * All it needs to do is exist for any amount of time.
 */
class Entity 
{
public:
    Entity(
        const std::string& name,    /** @brief Names don't have to be unique, they are only decorative. */
        uint64_t uid);              /** @brief A unique identifier -- useful for serialization. */
    ~Entity();

    const std::string& GetName();
    uint64_t GetUID();
    
    virtual void OnBorn() {}
    virtual void OnDeath() {}
    virtual void OnUpdate(float /* deltaTime */) {}
    virtual void OnRender(RenderData&  /* renderData */) {}

    virtual void SerializeState(SerializeData&) {}
    virtual void DeserializeState(const DeserializeData& /* serialData */) {}

protected:
    void SetName(const std::string& name);
    void SetUID(uint64_t uid);

private:
    std::string _name;
    uint64_t _uid;
};

} // namespace bl