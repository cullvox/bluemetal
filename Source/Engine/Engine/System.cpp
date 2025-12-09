#include "System.h"

namespace bl {

std::shared_ptr<Resource> System::ConstructResource(ResourceSystem&, std::size_t, const std::filesystem::path&) {
    // Default implementation returns nullptr
    return nullptr;
}

}