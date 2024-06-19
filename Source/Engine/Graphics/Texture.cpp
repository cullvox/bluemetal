#include "Texture.h"

#include "qoixx.hpp"
#include <cstddef>
#include <fstream>

namespace bl {

Texture::Texture(const nlohmann::json& data)
    : Resource(data) {
    
}

Texture::~Texture() {}

void Texture::Load() {
    
    std::ifstream file{GetPath(), std::ios::in | std::ios::binary};
    if (file.bad()) {
        
    }

    
    const auto [actual, desc] = qoixx::qoi::decode<std::vector<std::byte>>();

}

void Texture::Unload() {

}

} // namespace bl