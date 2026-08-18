#pragma once

#include "Engine/Engine.h"
#include "Core/ClassDB.h"
#include "Math/Math.h"

namespace bl {

class Object;

struct EnumValue
{
    std::string_view type;
    int64_t value;
};

using Variant = std::variant<
    Object*, 
    EnumValue,
    int64_t,
    bool,
    float, 
    double, 
    glm::vec2, 
    glm::vec3, 
    glm::vec4, 
    glm::quat, 
    glm::mat4, 
    std::string,
    std::vector<Object*>,
    std::vector<int64_t>,
    std::vector<float>,
    std::vector<double>,
    std::vector<glm::vec2>,
    std::vector<glm::vec3>,
    std::vector<glm::vec4>,
    std::vector<glm::quat>,
    std::vector<glm::mat4>,
    std::vector<std::string>>;

template<typename VariantType, typename T, std::size_t index = 0>
constexpr std::size_t VariantTypeIndex() {
    static_assert(std::variant_size_v<VariantType> > index, "Type not found in variant");

    // Support object pointers as a special case since we want to be able to use them for any object type.
    if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, Object*>) {
        if constexpr (std::is_pointer_v<T> && std::is_base_of_v<Object, std::remove_pointer_t<T>>) {
            return index;
        } else {
            return VariantTypeIndex<VariantType, T, index + 1>();
        }
    } else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
        return index;
    } else {
        return VariantTypeIndex<VariantType, T, index + 1>();
    }
}

enum class VariantType
{
    eObject           = VariantTypeIndex<Variant, Object*>(),
    eEnumeration      = VariantTypeIndex<Variant, EnumValue>(),
    eInteger          = VariantTypeIndex<Variant, int64_t>(),
    eBoolean          = VariantTypeIndex<Variant, bool>(),
    eFloat =            VariantTypeIndex<Variant, float>(),
    eDouble =           VariantTypeIndex<Variant, double>(),
    eVector2 =          VariantTypeIndex<Variant, glm::vec2>(),
    eVector3 =          VariantTypeIndex<Variant, glm::vec3>(),
    eVector4 =          VariantTypeIndex<Variant, glm::vec4>(),
    eQuaternion =       VariantTypeIndex<Variant, glm::quat>(),
    eMatrix4 =          VariantTypeIndex<Variant, glm::mat4>(),
    eString =           VariantTypeIndex<Variant, std::string>(),
    eObjectArray =      VariantTypeIndex<Variant, std::vector<Object*>>(),
    eIntegerArray =     VariantTypeIndex<Variant, std::vector<int64_t>>(),
    eFloatArray =       VariantTypeIndex<Variant, std::vector<float>>(),
    eDoubleArray =      VariantTypeIndex<Variant, std::vector<double>>(),
    eVector2Array =     VariantTypeIndex<Variant, std::vector<glm::vec2>>(),
    eVector3Array =     VariantTypeIndex<Variant, std::vector<glm::vec3>>(),
    eVector4Array =     VariantTypeIndex<Variant, std::vector<glm::vec4>>(),
    eQuaternionArray =  VariantTypeIndex<Variant, std::vector<glm::quat>>(),
    eMatrix4Array =     VariantTypeIndex<Variant, std::vector<glm::mat4>>(),
    eStringArray =      VariantTypeIndex<Variant, std::vector<std::string>>(),
};

template<typename T>
constexpr VariantType GetVariantType()
{
    return static_cast<VariantType>(VariantTypeIndex<Variant, T>());
}

}

namespace nlohmann {

template<>
struct adl_serializer<bl::EnumValue> {

    static void to_json(json& j, bl::EnumValue const& v) {
        j["type"] = v.type;
        j["value"] = bl::ClassDB::Get()->GetEnumValueName(v.type, v.value);
    }
 
    static void from_json(json const& j, bl::EnumValue& v) {
        const bl::EnumData* e = bl::ClassDB::Get()->FindEnum(j.value<std::string>("type", ""));
        if (!e) {
            throw std::runtime_error("Could not serialize an invalid enum type to.");
        }

        v.type = e->GetEnumName();
        v.value = j.value<uint64_t>("value", 0);
    }
};

template <typename ...Args>
struct adl_serializer<std::variant<Args...>> {
    static void to_json(json& j, std::variant<Args...> const& v) {
        std::visit([&](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, bl::Object*> || std::is_same_v<T, std::vector<bl::Object*>>) {
                throw std::runtime_error("Cannot serialize a object pointer to json!");
            } else {
                j = std::forward<decltype(value)>(value);
            }
        }, v);
    }

    static void from_json(json const& j, std::variant<Args...>& v) {
        
        // If json is an object it can only be so many things.
        if (j.is_object()) {

        }

        // If json is an array it can only be so many things.
        if (j.is_array()) {
        }

    }
};

}