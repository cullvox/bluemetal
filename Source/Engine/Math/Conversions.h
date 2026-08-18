#pragma once

namespace nlohmann {

template<>
struct adl_serializer<glm::vec2> {
    static void to_json(json& j, const glm::vec2& v) {
        j["x"] = v.x;
        j["y"] = v.y;
    }

    static void from_json(json const& j, glm::vec2& v) {
        v.x = j.value("x", 0.0f);
        v.y = j.value("y", 0.0f);
    }
};

template<>
struct adl_serializer<glm::vec3> {
    static void to_json(json& j, const glm::vec3& v) {
        j["x"] = v.x;
        j["y"] = v.y;
        j["z"] = v.z;
    }

    static void from_json(json const& j, glm::vec3& v) {
        v.x = j.value("x", 0.0f);
        v.y = j.value("y", 0.0f);
        v.z = j.value("z", 0.0f);
    }
};

template<>
struct adl_serializer<glm::vec4> {
    static void to_json(json& j, const glm::vec4& v) {
        j["x"] = v.x;
        j["y"] = v.y;
        j["z"] = v.z;
        j["w"] = v.w;
    }

    static void from_json(json const& j, glm::vec4& v) {
        v.x = j.value("x", 0.0f);
        v.y = j.value("y", 0.0f);
        v.z = j.value("z", 0.0f);
        v.w = j.value("w", 0.0f);
    }
};

template<>
struct adl_serializer<glm::quat> {
    static void to_json(json& j, const glm::quat& v) {
        j["x"] = v.x;
        j["y"] = v.y;
        j["z"] = v.z;
        j["w"] = v.w;
    }

    static void from_json(json const& j, glm::quat& v) {
        v.x = j.value("x", 0.0f);
        v.y = j.value("y", 0.0f);
        v.z = j.value("z", 0.0f);
        v.w = j.value("w", 0.0f);
    }
};

template<>
struct adl_serializer<glm::mat4> {
    static void to_json(json& j, const glm::mat4& v) {
        for (int i = 0; i < 4; i++)
            for (int k = 0; k < 4; k++)
                j[i][k] = v[i][k];
    }

    static void from_json(json const& j, glm::mat4& v) {
        v = glm::identity<glm::mat4>();
        try {
            for (int i = 0; i < 4; i++)
                for (int k = 0; k < 4; k++)
                    v[i][k] = j.at(i).at(k).get<float>();
        } catch(...) {};
    }
};

}