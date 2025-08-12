
#pragma once

#include "ClassRegister.h"
#include "Node3D.h"
#include "MaterialInstance.h"

class Member
{
    std::string _name;
public:
    virtual std::string GetName();
};

class Property
{
    
};

class Function
{

}

std::unordered_map<std::string, MemberBase>

#define REGISTER_CLASS(CLASS, PARENT) \
    CLASS::RegisterMembers();


class MeshInstance3D : public Node3D {
    REGISTER_CLASS(MeshInstance3D, Node3D)
public:
    MeshInstance3D() = default;
    virtual ~MeshInstance3D() = default;

    static void RegisterMembers();

    void SetMesh();
    void SetMaterial(Ref<MaterialInstance> material);
    virtual void Draw() override;
};