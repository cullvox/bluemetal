#include "Variant.h"
#include "ClassDB.h"

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