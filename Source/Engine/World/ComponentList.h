#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "Entity.h"
#include "Component.h"

namespace bl
{

class IComponentList
{
    void add(Entity id);
    void remove(Entity id);
};

template<typename TComponent>
class ComponentList : public IComponentList
{
public:
    static_assert(std::is_base_of_t<TComponent, Component>() == true)

private:
    std::unordered_map<Entity, int> m_entityToIndex;
    std::

};

} // namespace bl