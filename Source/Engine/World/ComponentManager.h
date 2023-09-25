#pragma once

#include "Precompiled.h"

namespace bl
{



class ComponentManager
{
public:
    ComponentManager();
    ~ComponentManager();

    template<typename TComponent>
    void registerComponent();

private:

    std::array<kMaxComponents, std::unique_ptr<IComponentList>> m_lists;
}

}