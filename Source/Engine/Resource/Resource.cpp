#include "Resource.h"

namespace bl
{

Resource::Resource(const nlohmann::json&) {}

std::filesystem::path Resource::GetPath() const {
    return _path;
}

uint64_t Resource::GetVersion() const {
    return _version;
}

ResourceLoadOp Resource::GetLoadOp() const {
    return _loadOp;
}

ResourceState Resource::GetState() const {
    return _state;
}

void Resource::Load() {
    _state = ResourceState::eLoaded;
}

void Resource::Unload() {
    _state = ResourceState::eUnloaded;
}

void Resource::SetPath(const std::filesystem::path& path) {
    _path = path;
}

void Resource::SetVersion(uint64_t version) {
    _version = version;
}

void Resource::SetLoadOp(ResourceLoadOp op) {
    _loadOp = op;
}

void Resource::SetState(ResourceState state) {
    _state = state;
}

} // namespace bl